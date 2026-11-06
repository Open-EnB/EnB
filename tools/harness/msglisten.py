"""Listens to a running client's debug-message server (msgserv.cpp, started by the client's -ENABLE_MESSAGES).

    python tools/harness/msglisten.py [--pid PID | --port P] [--host H] [--out FILE] [--seconds N]
                                      [--levels MASK] [--modules A,B] [--files F,G] [--prefix TEXT] [--raw]

The client's side (app/shared/message/msgserv.cpp, app/shared/message*.cpp): MsgServer listens on
the first free TCP port of 9999-10009 (TCPMgr::addListener(0, port): every interface, the client's own
choice) and, every 200 ms as a kernel task (and after each error report), accepts listeners (at most once a
second), reads their filters and sends them the queued messages. Its reporters drop every message while no
listener is connected, so nothing from before the connection (the client's start-up asserts) arrives.

The wire, both ways '\\0'-ended text records:
    listener -> client  'S' name '`' levels '`' modules '`' files '`' prefix     (one filter; 0x4BE7A0)
    client -> listener  'M' level '`' [sender ','] category '`' file '`' line '`' x '`' y '`' sec '.' usec '`' text
The filter's fields may not be empty (Wstring::getToken skips empty ones): ' ' means any. levels is a mask:
1 debug, 2 info, 4 warn, 8 error (G_ASSERT / FAIL_IF go through 0x4BC260, level 8), 0x10 stat (a debug
window's line at x, y), 0x20 0x4BC3A0's; 0xFFFFFF clears the listener's filters. A message reaches a listener
when one of its filters takes it (0x4BE2C0: the level, then one of the modules among sender / category, one of
the files, the text's prefix).

--pid PID finds the port the client listens on (netstat -ano; several clients each get their own port) and
waits for it; --port P (or neither: 9999) connects there. It retries the connection until the server is up and
reconnects while the PID lives. Each message is printed (and appended to --out) as
    HH:MM:SS.uuuuuu LEVEL [sender,]category: text  file(line)[ @x,y]   ('-' for neither)
--raw writes the records as received instead. It stops after --seconds N (default: when the connection ends and
the PID, if given, has gone).
"""
import argparse
import datetime
import socket
import subprocess
import time

PORTS = range(9999, 10010)
LEVELS = {1: 'DEBUG', 2: 'INFO', 4: 'WARN', 8: 'ERROR', 0x10: 'STAT', 0x20: 'L20'}


def pid_alive(pid):
    out = subprocess.run(['tasklist', '/FI', f'PID eq {pid}', '/NH'], capture_output=True, text=True).stdout
    return str(pid) in out


def pid_port(pid):
    """The port of 9999-10009 the process listens on, and the address it bound, or (None, None)."""
    out = subprocess.run(['netstat', '-ano', '-p', 'TCP'], capture_output=True, text=True).stdout
    for line in out.splitlines():
        f = line.split()
        if len(f) == 5 and f[3] == 'LISTENING' and f[4] == str(pid):
            addr, _, port = f[1].rpartition(':')
            if port.isdigit() and int(port) in PORTS:
                return int(port), addr
    return None, None


def decode(record):
    """'M'-record -> dict, or None."""
    if not record.startswith('M'):
        return None
    f = record[1:].split('`', 7)
    if len(f) < 8:
        return None
    level, who, file, line, x, y, when, text = f
    sender, _, category = who.rpartition(',')
    if who == 'NULL':
        category = ''
    try:
        sec, _, usec = when.partition('.')
        t = datetime.datetime.fromtimestamp(int(sec)).strftime('%H:%M:%S') + '.' + usec.rjust(6, '0')
    except ValueError:
        t = when
    try:
        level = int(level)
    except ValueError:
        pass
    return {'time': t, 'level': level, 'sender': sender, 'category': category, 'file': file,
            'line': line, 'x': x, 'y': y, 'text': text.rstrip('\r\n')}


def format_message(m):
    who = ','.join(x for x in (m['sender'], m['category']) if x) or '-'
    where = f'  {m["file"]}({m["line"]})' if m['file'] != 'NULL' else ''
    at = f' @{m["x"]},{m["y"]}' if m['level'] == 0x10 else ''
    return f'{m["time"]} {LEVELS.get(m["level"], m["level"]):5} {who}: {m["text"]}{where}{at}'


def filter_record(a):
    def field(v):
        return v if v else ' '
    return f'S{a.name}`{a.levels}`{field(a.modules)}`{field(a.files)}`{field(a.prefix)}'.encode('latin1') + b'\0'


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--pid', type=int, help="the client's process: find its port (netstat) and wait for it")
    ap.add_argument('--port', type=int, help='the port (default 9999 without --pid)')
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--out', help='append the decoded messages to this file too')
    ap.add_argument('--seconds', type=float, help='stop after N seconds')
    ap.add_argument('--levels', type=lambda s: int(s, 0), default=0x3F, help='level mask (default 0x3F: all)')
    ap.add_argument('--modules', default='', help="senders / categories, ',' separated (default: any)")
    ap.add_argument('--files', default='', help="__FILE__ names without directories, ',' separated (default: any)")
    ap.add_argument('--prefix', default='', help="the text's start (default: any)")
    ap.add_argument('--name', default='msglisten', help="the filter's name")
    ap.add_argument('--raw', action='store_true', help='write the records as received')
    a = ap.parse_args()
    end = time.time() + a.seconds if a.seconds else None
    out = open(a.out, 'a', encoding='utf-8', newline='\n') if a.out else None

    def emit(text):
        print(text, flush=True)
        if out:
            out.write(text + '\n')
            out.flush()

    def running():
        return (end is None or time.time() < end)

    count = 0
    told = False
    while running():
        port = a.port
        if a.pid:
            if not pid_alive(a.pid):
                emit(f'# msglisten: PID {a.pid} has gone')
                break
            if port is None:
                port, addr = pid_port(a.pid)
                if port is None:
                    time.sleep(0.2)
                    continue
                if not told:
                    emit(f'# msglisten: PID {a.pid} listens on {addr}:{port}')
                    told = True
        port = port or 9999
        try:
            s = socket.create_connection((a.host, port), timeout=1)
        except OSError:
            time.sleep(0.2)
            continue
        emit(f'# msglisten: connected to {a.host}:{port}, filter levels 0x{a.levels:X}')
        s.sendall(filter_record(a))
        s.settimeout(0.5)
        buf = b''
        while running():
            try:
                data = s.recv(65536)
            except socket.timeout:
                continue
            except OSError:
                data = b''
            if not data:
                emit('# msglisten: connection closed')
                break
            buf += data
            while b'\0' in buf:
                rec, buf = buf.split(b'\0', 1)
                rec = rec.decode('latin1')
                m = decode(rec)
                count += 1
                emit(rec if a.raw or m is None else format_message(m))
        s.close()
        if not a.pid:
            break
    emit(f'# msglisten: {count} messages')
    if out:
        out.close()


if __name__ == '__main__':
    main()

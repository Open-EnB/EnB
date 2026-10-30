"""Local settings of the build VMs: the one place vc6.py, pool.py and the setup scripts read them.

Defaults suit a VM made by tools/vc6/setup/New-BuildVM.ps1 (docs/setup-vc6.md). A machine whose VMs
differ writes `.work/vc6_config.json` (git-ignored; New-BuildVM.ps1 writes it), and an environment
variable overrides either:

    setting       variable            meaning                   default
    (the file)    ENB_VC6_CONFIG      this settings file        .work/vc6_config.json
    user         ENB_VC6_USER        VM account for SSH        user
    key           ENB_VC6_KEY         SSH private key           ~/.ssh/enb_vc6
    pool_dir      ENB_VC6_POOL_DIR    pool disks and the base   C:\\EnBDevelopment\\Tools\\VMPool
    template_vm   ENB_VC6_TEMPLATE    VM whose hardware clones copy (never started by the pool)
                                                                enb-vc6-template
    prefix        ENB_VC6_PREFIX      pool VM name prefix       enb-vc6-
    max_vms       ENB_VC6_MAX         pool cap                  7
    reserve_gb    ENB_VC6_RESERVE_GB  host memory a new clone must leave free   4
    switch        ENB_VC6_SWITCH      Hyper-V switch            Default Switch
"""
import json
import os

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEFAULTS = {
    'user': 'user',
    'key': '~/.ssh/enb_vc6',
    'pool_dir': r'C:\EnBDevelopment\Tools\VMPool',
    'template_vm': 'enb-vc6-template',
    'prefix': 'enb-vc6-',
    'max_vms': 7,
    'reserve_gb': 4,
    'switch': 'Default Switch',
}
ENV = {'user': 'ENB_VC6_USER', 'key': 'ENB_VC6_KEY', 'pool_dir': 'ENB_VC6_POOL_DIR',
       'template_vm': 'ENB_VC6_TEMPLATE', 'prefix': 'ENB_VC6_PREFIX', 'max_vms': 'ENB_VC6_MAX',
       'reserve_gb': 'ENB_VC6_RESERVE_GB', 'switch': 'ENB_VC6_SWITCH'}
def main_checkout(repo):
    """The main working tree of `repo`: itself, or for a linked worktree (wt.py's .work/wt/NAME,
    whose .git is a file `gitdir: <main>/.git/worktrees/NAME`) the main one, whose .work holds
    the machine's settings."""
    dot_git = os.path.join(repo, '.git')
    if os.path.isfile(dot_git):
        with open(dot_git, encoding='utf-8') as f:
            line = f.read().strip()
        if line.startswith('gitdir:'):
            gitdir = os.path.normpath(line[len('gitdir:'):].strip())
            parts = gitdir.replace('\\', '/').split('/')
            if 'worktrees' in parts:
                return os.path.dirname(os.path.normpath('/'.join(parts[:parts.index('worktrees')])))
    return repo


CONFIG_FILE = os.environ.get('ENB_VC6_CONFIG',
                             os.path.join(main_checkout(REPO), '.work', 'vc6_config.json'))


def load():
    cfg = dict(DEFAULTS)
    if os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, encoding='utf-8-sig') as f:
            cfg.update({k: v for k, v in json.load(f).items() if k in DEFAULTS})
    for k, var in ENV.items():
        if os.environ.get(var):
            cfg[k] = os.environ[var]
    for k in ('max_vms', 'reserve_gb'):
        cfg[k] = int(cfg[k])
    cfg['key'] = os.path.expanduser(cfg['key'])
    return cfg


CFG = load()

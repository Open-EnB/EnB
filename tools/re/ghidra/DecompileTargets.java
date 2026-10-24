// Headless Ghidra script for tools/re/pseudocode.py (not for the Script Manager).
// Args: NAMES TARGETS OUTDIR [TIMEOUT_S]. NAMES and TARGETS are tab-separated files, one
// "ADDR(hex)<TAB>name" per line. Creates a function at every listed address first (Ghidra's
// auto-analysis never finds methods reached only through a vtable), names the known ones, then
// writes each target's pseudocode to OUTDIR/XXXXXXXX.c and a status line per target to
// OUTDIR/index-new.tsv.
// @category EnB
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;

import ghidra.app.decompiler.*;
import ghidra.app.decompiler.parallel.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import ghidra.util.task.TaskMonitor;

public class DecompileTargets extends GhidraScript {

	private List<String[]> read(Path p) throws IOException {
		List<String[]> rows = new ArrayList<>();
		for (String line : Files.readAllLines(p, StandardCharsets.UTF_8)) {
			if (!line.isEmpty())
				rows.add(line.split("\t", 2));
		}
		return rows;
	}

	private Function ensure(Address a) {
		Function f = getFunctionAt(a);
		return f != null ? f : createFunction(a, null);
	}

	@Override
	public void run() throws Exception {
		String[] args = getScriptArgs();
		List<String[]> names = read(Paths.get(args[0]));
		List<String[]> targets = read(Paths.get(args[1]));
		Path out = Paths.get(args[2]);
		int timeout = args.length > 3 ? Integer.parseInt(args[3]) : 60;
		Files.createDirectories(out);

		// Every function first, so the pseudocode of one names its calls into the others.
		int named = 0;
		for (String[] r : names) {
			Address a = toAddr(Long.parseLong(r[0], 16));
			Function f = ensure(a);
			try {
				String n = r[1].replace(' ', '_');
				if (f != null)
					f.setName(n, SourceType.IMPORTED);
				else
					currentProgram.getSymbolTable().createLabel(a, n, SourceType.IMPORTED);
				named++;
			}
			catch (Exception e) {
				// duplicate or invalid name: keep Ghidra's
			}
		}
		Map<Function, String> todo = new LinkedHashMap<>();
		List<String> status = Collections.synchronizedList(new ArrayList<>());
		for (String[] r : targets) {
			Function f = ensure(toAddr(Long.parseLong(r[0], 16)));
			if (f == null)
				status.add(r[0] + "\tno_function\tGhidra could not create a function here");
			else
				todo.put(f, r[0]);
		}
		println("named " + named + " of " + names.size() + "; decompiling " + todo.size());

		DecompilerCallback<Void> cb = new DecompilerCallback<>(currentProgram, d -> {
			DecompileOptions o = new DecompileOptions();
			o.grabFromProgram(currentProgram);
			d.setOptions(o);
			d.toggleCCode(true);
			d.setSimplificationStyle("decompile");
		}) {
			@Override
			public Void process(DecompileResults res, TaskMonitor m) throws Exception {
				Function f = res.getFunction();
				String key = todo.get(f);
				if (res.decompileCompleted()) {
					String c = res.getDecompiledFunction().getC().replace("\r\n", "\n");
					Files.writeString(out.resolve(key + ".c"),
						"// " + key + " " + f.getName() + " (Ghidra " +
							getGhidraVersion() + ", tools/re/pseudocode.py)\n" + c,
						StandardCharsets.UTF_8);
					status.add(key + "\tok\t" + c.length());
				}
				else {
					String msg = String.valueOf(res.getErrorMessage()).replaceAll("\\s+", " ").trim();
					status.add(key + "\tfailed\t" + msg);
				}
				return null;
			}
		};
		cb.setTimeout(timeout);
		try {
			ParallelDecompiler.decompileFunctions(cb, todo.keySet(), monitor);
		}
		finally {
			cb.dispose();
			Files.write(out.resolve("index-new.tsv"), status, StandardCharsets.UTF_8);
		}
	}
}

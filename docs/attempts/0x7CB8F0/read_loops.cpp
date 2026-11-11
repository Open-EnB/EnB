// 0x7CB8F0, the read loop (the rest of the function is the client's). The client: the count zeroed and
// InternetReadFile called once, at the loop's top, an unconditional jmp back after buffer[0] = 0.
// Form 1 (0.68; rotated, the read twice, the count zeroed once before the loop):
//	DWORD read = 0;
//	while (InternetReadFile(request, buffer, sizeof(buffer), &read) && read != 0) { reply.append(buffer); buffer[0] = '\0'; }
// Form 2 (0.66; rotated, the zero and the read twice):
//	for (;;) { DWORD read = 0; if (!InternetReadFile(request, buffer, sizeof(buffer), &read) || read == 0) break; reply.append(buffer); buffer[0] = '\0'; }
// Levers that were needed and are in the tree: reply.append(buffer) (not append(buffer, strlen(buffer)): _Eos
// out of line), substr(0, 7).compare("Success") == 0 (not ==: compare expanded).
// Form 3 (0.77, the tree's; order-only, the same 231 instructions): the client's unrotated loop comes from a flag:
//	bool more = true;
//	while (more) { DWORD read = 0; if (InternetReadFile(...) && read != 0) { reply.append(buffer); buffer[0] = '\0'; } else { more = false; } }
// Residue: the client stores the zero before the call's lea / pushes, ours between the pushes and the call; and
// `mov esi, [esp + 0x28]` sits after the first InternetCloseHandle in the client, before it in ours.

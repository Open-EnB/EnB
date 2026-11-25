// Earth & Beyond client (reconstruction)
/*
** mss32log: an MSS32.DLL proxy that logs the client's Miles calls as JSON lines (test-plan C2).
**
** The harness (clientrun.Inst.use_audio_log) copies the game's Mss32.dll into an instance folder as
** mss32r.dll and this proxy over Mss32.dll. The client's import table binds to the proxy; DllMain loads
** mss32r.dll from the proxy's own folder and resolves every one of its 344 exports. Every export of the
** real DLL (Miles 6.1a) is exported here under the same decorated name: the ones in the generated block
** below are naked jumps through the resolved pointer (no frame, nothing touched: any calling convention,
** any arguments), the ones logged are written out with mss.h 6.1a's prototypes (SDK/radtools/include_enb),
** whose argument bytes the decorated names confirm (the build compares this DLL's export names with the
** real one's: a wrong argument count changes the name).
**
** Only calls made from the main exe's image are logged: Miles' own providers (mssfast.m3d imports
** _AIL_start_sample@4 and others from "mss32.dll", which is this proxy) pass through unlogged.
**
** Environment:
**   MSS32LOG_FILE  the log file (JSON lines). Unset: nothing is logged and nothing is changed; set or not,
**                  every call reaches the real DLL with its own arguments and returns its result.
**
** The log: one JSON object per line, keys in a fixed order: "i" (event index, 0 = the header), "t" (ms
** since the log opened, QueryPerformanceCounter), "tick" (GetTickCount, the d3d8shot.log stamp's clock),
** "f" (the D3D frame when the d3d8 proxy exports D3d8shot_Frame, else absent), "ev", then the event's
** fields. Handles are ids in order of creation, never pointers: d1 driver, p1 3D provider, f1 filter,
** l1 listener, s1 2D sample, t1 3D sample, m1 stream, u1 a handle the log never saw created. A loaded
** memory sample is named by its content: "size" and "hash" (FNV-1a 64 of the bytes, hex); a stream by its
** file name. Events that start, stop or change a sound carry "snd", the handle's current sound.
** tools/harness/audiolog.py reads it (summary, sequence, diff); README.md "Audio log" has the schema.
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

/* ------------------------------------------------------------------ the forwarded exports (generated) */
#define D __declspec(dllexport)
/* GENERATED-BEGIN: python tools/harness/audiolog.py gen (from the game's Mss32.dll exports) */
static FARPROC p_stream_background;
D __declspec(naked) void __fastcall stream_background(void) { __asm jmp dword ptr [p_stream_background] }
static FARPROC p_AIL_3D_orientation;
D __declspec(naked) void __stdcall AIL_3D_orientation(int a0, int a1, int a2, int a3, int a4, int a5, int a6) { __asm jmp dword ptr [p_AIL_3D_orientation] }
static FARPROC p_AIL_3D_position;
D __declspec(naked) void __stdcall AIL_3D_position(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_3D_position] }
static FARPROC p_AIL_3D_provider_attribute;
D __declspec(naked) void __stdcall AIL_3D_provider_attribute(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_3D_provider_attribute] }
static FARPROC p_AIL_3D_room_type;
D __declspec(naked) void __stdcall AIL_3D_room_type(int a0) { __asm jmp dword ptr [p_AIL_3D_room_type] }
static FARPROC p_AIL_3D_sample_attribute;
D __declspec(naked) void __stdcall AIL_3D_sample_attribute(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_3D_sample_attribute] }
static FARPROC p_AIL_3D_sample_cone;
D __declspec(naked) void __stdcall AIL_3D_sample_cone(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_3D_sample_cone] }
static FARPROC p_AIL_3D_sample_distances;
D __declspec(naked) void __stdcall AIL_3D_sample_distances(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_3D_sample_distances] }
static FARPROC p_AIL_3D_sample_effects_level;
D __declspec(naked) void __stdcall AIL_3D_sample_effects_level(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_effects_level] }
static FARPROC p_AIL_3D_sample_length;
D __declspec(naked) void __stdcall AIL_3D_sample_length(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_length] }
static FARPROC p_AIL_3D_sample_loop_count;
D __declspec(naked) void __stdcall AIL_3D_sample_loop_count(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_loop_count] }
static FARPROC p_AIL_3D_sample_obstruction;
D __declspec(naked) void __stdcall AIL_3D_sample_obstruction(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_obstruction] }
static FARPROC p_AIL_3D_sample_occlusion;
D __declspec(naked) void __stdcall AIL_3D_sample_occlusion(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_occlusion] }
static FARPROC p_AIL_3D_sample_offset;
D __declspec(naked) void __stdcall AIL_3D_sample_offset(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_offset] }
static FARPROC p_AIL_3D_sample_playback_rate;
D __declspec(naked) void __stdcall AIL_3D_sample_playback_rate(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_playback_rate] }
static FARPROC p_AIL_3D_sample_status;
D __declspec(naked) void __stdcall AIL_3D_sample_status(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_status] }
static FARPROC p_AIL_3D_sample_volume;
D __declspec(naked) void __stdcall AIL_3D_sample_volume(int a0) { __asm jmp dword ptr [p_AIL_3D_sample_volume] }
static FARPROC p_AIL_3D_speaker_type;
D __declspec(naked) void __stdcall AIL_3D_speaker_type(int a0) { __asm jmp dword ptr [p_AIL_3D_speaker_type] }
static FARPROC p_AIL_3D_user_data;
D __declspec(naked) void __stdcall AIL_3D_user_data(int a0, int a1) { __asm jmp dword ptr [p_AIL_3D_user_data] }
static FARPROC p_AIL_3D_velocity;
D __declspec(naked) void __stdcall AIL_3D_velocity(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_3D_velocity] }
static FARPROC p_AIL_DLS_close;
D __declspec(naked) void __stdcall AIL_DLS_close(int a0, int a1) { __asm jmp dword ptr [p_AIL_DLS_close] }
static FARPROC p_AIL_DLS_compact;
D __declspec(naked) void __stdcall AIL_DLS_compact(int a0) { __asm jmp dword ptr [p_AIL_DLS_compact] }
static FARPROC p_AIL_DLS_get_info;
D __declspec(naked) void __stdcall AIL_DLS_get_info(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_DLS_get_info] }
static FARPROC p_AIL_DLS_get_reverb;
D __declspec(naked) void __stdcall AIL_DLS_get_reverb(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_DLS_get_reverb] }
static FARPROC p_AIL_DLS_load_file;
D __declspec(naked) void __stdcall AIL_DLS_load_file(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_DLS_load_file] }
static FARPROC p_AIL_DLS_load_memory;
D __declspec(naked) void __stdcall AIL_DLS_load_memory(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_DLS_load_memory] }
static FARPROC p_AIL_DLS_open;
D __declspec(naked) void __stdcall AIL_DLS_open(int a0, int a1, int a2, int a3, int a4, int a5, int a6) { __asm jmp dword ptr [p_AIL_DLS_open] }
static FARPROC p_AIL_DLS_set_reverb;
D __declspec(naked) void __stdcall AIL_DLS_set_reverb(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_DLS_set_reverb] }
static FARPROC p_AIL_DLS_unload;
D __declspec(naked) void __stdcall AIL_DLS_unload(int a0, int a1) { __asm jmp dword ptr [p_AIL_DLS_unload] }
static FARPROC p_AIL_HWND;
D __declspec(naked) void __stdcall AIL_HWND(void) { __asm jmp dword ptr [p_AIL_HWND] }
static FARPROC p_AIL_MIDI_handle_reacquire;
D __declspec(naked) void __stdcall AIL_MIDI_handle_reacquire(int a0) { __asm jmp dword ptr [p_AIL_MIDI_handle_reacquire] }
static FARPROC p_AIL_MIDI_handle_release;
D __declspec(naked) void __stdcall AIL_MIDI_handle_release(int a0) { __asm jmp dword ptr [p_AIL_MIDI_handle_release] }
static FARPROC p_AIL_MIDI_to_XMI;
D __declspec(naked) void __stdcall AIL_MIDI_to_XMI(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_MIDI_to_XMI] }
static FARPROC p_AIL_MMX_available;
D __declspec(naked) void __stdcall AIL_MMX_available(void) { __asm jmp dword ptr [p_AIL_MMX_available] }
static FARPROC p_AIL_WAV_file_write;
D __declspec(naked) void __stdcall AIL_WAV_file_write(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_WAV_file_write] }
static FARPROC p_AIL_WAV_info;
D __declspec(naked) void __stdcall AIL_WAV_info(int a0, int a1) { __asm jmp dword ptr [p_AIL_WAV_info] }
static FARPROC p_AIL_XMIDI_master_volume;
D __declspec(naked) void __stdcall AIL_XMIDI_master_volume(int a0) { __asm jmp dword ptr [p_AIL_XMIDI_master_volume] }
static FARPROC p_AIL_active_3D_sample_count;
D __declspec(naked) void __stdcall AIL_active_3D_sample_count(int a0) { __asm jmp dword ptr [p_AIL_active_3D_sample_count] }
static FARPROC p_AIL_active_sample_count;
D __declspec(naked) void __stdcall AIL_active_sample_count(int a0) { __asm jmp dword ptr [p_AIL_active_sample_count] }
static FARPROC p_AIL_active_sequence_count;
D __declspec(naked) void __stdcall AIL_active_sequence_count(int a0) { __asm jmp dword ptr [p_AIL_active_sequence_count] }
static FARPROC p_AIL_allocate_3D_sample_handle;
static FARPROC p_AIL_allocate_file_sample;
D __declspec(naked) void __stdcall AIL_allocate_file_sample(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_allocate_file_sample] }
static FARPROC p_AIL_allocate_sample_handle;
static FARPROC p_AIL_allocate_sequence_handle;
D __declspec(naked) void __stdcall AIL_allocate_sequence_handle(int a0) { __asm jmp dword ptr [p_AIL_allocate_sequence_handle] }
static FARPROC p_AIL_auto_service_stream;
D __declspec(naked) void __stdcall AIL_auto_service_stream(int a0, int a1) { __asm jmp dword ptr [p_AIL_auto_service_stream] }
static FARPROC p_AIL_auto_update_3D_position;
D __declspec(naked) void __stdcall AIL_auto_update_3D_position(int a0, int a1) { __asm jmp dword ptr [p_AIL_auto_update_3D_position] }
static FARPROC p_AIL_background;
D __declspec(naked) void __stdcall AIL_background(void) { __asm jmp dword ptr [p_AIL_background] }
static FARPROC p_AIL_branch_index;
D __declspec(naked) void __stdcall AIL_branch_index(int a0, int a1) { __asm jmp dword ptr [p_AIL_branch_index] }
static FARPROC p_AIL_channel_notes;
D __declspec(naked) void __stdcall AIL_channel_notes(int a0, int a1) { __asm jmp dword ptr [p_AIL_channel_notes] }
static FARPROC p_AIL_close_3D_listener;
static FARPROC p_AIL_close_3D_object;
D __declspec(naked) void __stdcall AIL_close_3D_object(int a0) { __asm jmp dword ptr [p_AIL_close_3D_object] }
static FARPROC p_AIL_close_3D_provider;
static FARPROC p_AIL_close_XMIDI_driver;
D __declspec(naked) void __stdcall AIL_close_XMIDI_driver(int a0) { __asm jmp dword ptr [p_AIL_close_XMIDI_driver] }
static FARPROC p_AIL_close_digital_driver;
static FARPROC p_AIL_close_filter;
D __declspec(naked) void __stdcall AIL_close_filter(int a0) { __asm jmp dword ptr [p_AIL_close_filter] }
static FARPROC p_AIL_close_input;
D __declspec(naked) void __stdcall AIL_close_input(int a0) { __asm jmp dword ptr [p_AIL_close_input] }
static FARPROC p_AIL_close_stream;
static FARPROC p_AIL_compress_ADPCM;
D __declspec(naked) void __stdcall AIL_compress_ADPCM(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_compress_ADPCM] }
static FARPROC p_AIL_compress_ASI;
D __declspec(naked) void __stdcall AIL_compress_ASI(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_compress_ASI] }
static FARPROC p_AIL_compress_DLS;
D __declspec(naked) void __stdcall AIL_compress_DLS(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_compress_DLS] }
static FARPROC p_AIL_controller_value;
D __declspec(naked) void __stdcall AIL_controller_value(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_controller_value] }
static FARPROC p_AIL_create_wave_synthesizer;
D __declspec(naked) void __stdcall AIL_create_wave_synthesizer(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_create_wave_synthesizer] }
static FARPROC p_AIL_debug_printf;
D __declspec(naked) void __cdecl AIL_debug_printf(void) { __asm jmp dword ptr [p_AIL_debug_printf] }
static FARPROC p_AIL_decompress_ADPCM;
D __declspec(naked) void __stdcall AIL_decompress_ADPCM(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_decompress_ADPCM] }
static FARPROC p_AIL_decompress_ASI;
D __declspec(naked) void __stdcall AIL_decompress_ASI(int a0, int a1, int a2, int a3, int a4, int a5) { __asm jmp dword ptr [p_AIL_decompress_ASI] }
static FARPROC p_AIL_delay;
D __declspec(naked) void __stdcall AIL_delay(int a0) { __asm jmp dword ptr [p_AIL_delay] }
static FARPROC p_AIL_destroy_wave_synthesizer;
D __declspec(naked) void __stdcall AIL_destroy_wave_synthesizer(int a0) { __asm jmp dword ptr [p_AIL_destroy_wave_synthesizer] }
static FARPROC p_AIL_digital_CPU_percent;
D __declspec(naked) void __stdcall AIL_digital_CPU_percent(int a0) { __asm jmp dword ptr [p_AIL_digital_CPU_percent] }
static FARPROC p_AIL_digital_configuration;
D __declspec(naked) void __stdcall AIL_digital_configuration(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_digital_configuration] }
static FARPROC p_AIL_digital_handle_reacquire;
D __declspec(naked) void __stdcall AIL_digital_handle_reacquire(int a0) { __asm jmp dword ptr [p_AIL_digital_handle_reacquire] }
static FARPROC p_AIL_digital_handle_release;
D __declspec(naked) void __stdcall AIL_digital_handle_release(int a0) { __asm jmp dword ptr [p_AIL_digital_handle_release] }
static FARPROC p_AIL_digital_latency;
D __declspec(naked) void __stdcall AIL_digital_latency(int a0) { __asm jmp dword ptr [p_AIL_digital_latency] }
static FARPROC p_AIL_digital_master_volume;
D __declspec(naked) void __stdcall AIL_digital_master_volume(int a0) { __asm jmp dword ptr [p_AIL_digital_master_volume] }
static FARPROC p_AIL_end_3D_sample;
static FARPROC p_AIL_end_sample;
static FARPROC p_AIL_end_sequence;
D __declspec(naked) void __stdcall AIL_end_sequence(int a0) { __asm jmp dword ptr [p_AIL_end_sequence] }
static FARPROC p_AIL_enumerate_3D_provider_attributes;
D __declspec(naked) void __stdcall AIL_enumerate_3D_provider_attributes(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_enumerate_3D_provider_attributes] }
static FARPROC p_AIL_enumerate_3D_providers;
static FARPROC p_AIL_enumerate_3D_sample_attributes;
D __declspec(naked) void __stdcall AIL_enumerate_3D_sample_attributes(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_enumerate_3D_sample_attributes] }
static FARPROC p_AIL_enumerate_filter_attributes;
D __declspec(naked) void __stdcall AIL_enumerate_filter_attributes(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_enumerate_filter_attributes] }
static FARPROC p_AIL_enumerate_filter_sample_attributes;
D __declspec(naked) void __stdcall AIL_enumerate_filter_sample_attributes(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_enumerate_filter_sample_attributes] }
static FARPROC p_AIL_enumerate_filters;
static FARPROC p_AIL_extract_DLS;
D __declspec(naked) void __stdcall AIL_extract_DLS(int a0, int a1, int a2, int a3, int a4, int a5, int a6) { __asm jmp dword ptr [p_AIL_extract_DLS] }
static FARPROC p_AIL_file_error;
D __declspec(naked) void __stdcall AIL_file_error(void) { __asm jmp dword ptr [p_AIL_file_error] }
static FARPROC p_AIL_file_read;
D __declspec(naked) void __stdcall AIL_file_read(int a0, int a1) { __asm jmp dword ptr [p_AIL_file_read] }
static FARPROC p_AIL_file_size;
D __declspec(naked) void __stdcall AIL_file_size(int a0) { __asm jmp dword ptr [p_AIL_file_size] }
static FARPROC p_AIL_file_type;
D __declspec(naked) void __stdcall AIL_file_type(int a0, int a1) { __asm jmp dword ptr [p_AIL_file_type] }
static FARPROC p_AIL_file_write;
D __declspec(naked) void __stdcall AIL_file_write(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_file_write] }
static FARPROC p_AIL_filter_DLS_attribute;
D __declspec(naked) void __stdcall AIL_filter_DLS_attribute(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_filter_DLS_attribute] }
static FARPROC p_AIL_filter_DLS_with_XMI;
D __declspec(naked) void __stdcall AIL_filter_DLS_with_XMI(int a0, int a1, int a2, int a3, int a4, int a5) { __asm jmp dword ptr [p_AIL_filter_DLS_with_XMI] }
static FARPROC p_AIL_filter_attribute;
D __declspec(naked) void __stdcall AIL_filter_attribute(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_filter_attribute] }
static FARPROC p_AIL_filter_sample_attribute;
D __declspec(naked) void __stdcall AIL_filter_sample_attribute(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_filter_sample_attribute] }
static FARPROC p_AIL_filter_stream_attribute;
D __declspec(naked) void __stdcall AIL_filter_stream_attribute(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_filter_stream_attribute] }
static FARPROC p_AIL_find_DLS;
D __declspec(naked) void __stdcall AIL_find_DLS(int a0, int a1, int a2, int a3, int a4, int a5) { __asm jmp dword ptr [p_AIL_find_DLS] }
static FARPROC p_AIL_get_DirectSound_info;
D __declspec(naked) void __stdcall AIL_get_DirectSound_info(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_get_DirectSound_info] }
static FARPROC p_AIL_get_input_info;
D __declspec(naked) void __stdcall AIL_get_input_info(int a0) { __asm jmp dword ptr [p_AIL_get_input_info] }
static FARPROC p_AIL_get_preference;
D __declspec(naked) void __stdcall AIL_get_preference(int a0) { __asm jmp dword ptr [p_AIL_get_preference] }
static FARPROC p_AIL_get_timer_highest_delay;
D __declspec(naked) void __stdcall AIL_get_timer_highest_delay(void) { __asm jmp dword ptr [p_AIL_get_timer_highest_delay] }
static FARPROC p_AIL_init_sample;
static FARPROC p_AIL_init_sequence;
D __declspec(naked) void __stdcall AIL_init_sequence(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_init_sequence] }
static FARPROC p_AIL_last_error;
D __declspec(naked) void __stdcall AIL_last_error(void) { __asm jmp dword ptr [p_AIL_last_error] }
static FARPROC p_AIL_list_DLS;
D __declspec(naked) void __stdcall AIL_list_DLS(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_list_DLS] }
static FARPROC p_AIL_list_MIDI;
D __declspec(naked) void __stdcall AIL_list_MIDI(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_list_MIDI] }
static FARPROC p_AIL_load_sample_buffer;
D __declspec(naked) void __stdcall AIL_load_sample_buffer(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_load_sample_buffer] }
static FARPROC p_AIL_lock;
D __declspec(naked) void __stdcall AIL_lock(void) { __asm jmp dword ptr [p_AIL_lock] }
static FARPROC p_AIL_lock_channel;
D __declspec(naked) void __stdcall AIL_lock_channel(int a0) { __asm jmp dword ptr [p_AIL_lock_channel] }
static FARPROC p_AIL_lock_mutex;
D __declspec(naked) void __stdcall AIL_lock_mutex(void) { __asm jmp dword ptr [p_AIL_lock_mutex] }
static FARPROC p_AIL_map_sequence_channel;
D __declspec(naked) void __stdcall AIL_map_sequence_channel(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_map_sequence_channel] }
static FARPROC p_AIL_mem_alloc_lock;
D __declspec(naked) void __stdcall AIL_mem_alloc_lock(int a0) { __asm jmp dword ptr [p_AIL_mem_alloc_lock] }
static FARPROC p_AIL_mem_free_lock;
D __declspec(naked) void __stdcall AIL_mem_free_lock(int a0) { __asm jmp dword ptr [p_AIL_mem_free_lock] }
static FARPROC p_AIL_merge_DLS_with_XMI;
D __declspec(naked) void __stdcall AIL_merge_DLS_with_XMI(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_merge_DLS_with_XMI] }
static FARPROC p_AIL_midiOutClose;
D __declspec(naked) void __stdcall AIL_midiOutClose(int a0) { __asm jmp dword ptr [p_AIL_midiOutClose] }
static FARPROC p_AIL_midiOutOpen;
D __declspec(naked) void __stdcall AIL_midiOutOpen(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_midiOutOpen] }
static FARPROC p_AIL_minimum_sample_buffer_size;
D __declspec(naked) void __stdcall AIL_minimum_sample_buffer_size(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_minimum_sample_buffer_size] }
static FARPROC p_AIL_ms_count;
D __declspec(naked) void __stdcall AIL_ms_count(void) { __asm jmp dword ptr [p_AIL_ms_count] }
static FARPROC p_AIL_open_3D_listener;
static FARPROC p_AIL_open_3D_object;
D __declspec(naked) void __stdcall AIL_open_3D_object(int a0) { __asm jmp dword ptr [p_AIL_open_3D_object] }
static FARPROC p_AIL_open_3D_provider;
static FARPROC p_AIL_open_XMIDI_driver;
D __declspec(naked) void __stdcall AIL_open_XMIDI_driver(int a0) { __asm jmp dword ptr [p_AIL_open_XMIDI_driver] }
static FARPROC p_AIL_open_digital_driver;
static FARPROC p_AIL_open_filter;
D __declspec(naked) void __stdcall AIL_open_filter(int a0, int a1) { __asm jmp dword ptr [p_AIL_open_filter] }
static FARPROC p_AIL_open_input;
D __declspec(naked) void __stdcall AIL_open_input(int a0) { __asm jmp dword ptr [p_AIL_open_input] }
static FARPROC p_AIL_open_stream;
static FARPROC p_AIL_open_stream_by_sample;
static FARPROC p_AIL_pause_stream;
static FARPROC p_AIL_primary_digital_driver;
D __declspec(naked) void __stdcall AIL_primary_digital_driver(int a0) { __asm jmp dword ptr [p_AIL_primary_digital_driver] }
static FARPROC p_AIL_process_digital_audio;
D __declspec(naked) void __stdcall AIL_process_digital_audio(int a0, int a1, int a2, int a3, int a4, int a5) { __asm jmp dword ptr [p_AIL_process_digital_audio] }
static FARPROC p_AIL_quick_copy;
D __declspec(naked) void __stdcall AIL_quick_copy(int a0) { __asm jmp dword ptr [p_AIL_quick_copy] }
static FARPROC p_AIL_quick_halt;
D __declspec(naked) void __stdcall AIL_quick_halt(int a0) { __asm jmp dword ptr [p_AIL_quick_halt] }
static FARPROC p_AIL_quick_handles;
D __declspec(naked) void __stdcall AIL_quick_handles(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_quick_handles] }
static FARPROC p_AIL_quick_load;
D __declspec(naked) void __stdcall AIL_quick_load(int a0) { __asm jmp dword ptr [p_AIL_quick_load] }
static FARPROC p_AIL_quick_load_and_play;
D __declspec(naked) void __stdcall AIL_quick_load_and_play(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_quick_load_and_play] }
static FARPROC p_AIL_quick_load_mem;
D __declspec(naked) void __stdcall AIL_quick_load_mem(int a0, int a1) { __asm jmp dword ptr [p_AIL_quick_load_mem] }
static FARPROC p_AIL_quick_ms_length;
D __declspec(naked) void __stdcall AIL_quick_ms_length(int a0) { __asm jmp dword ptr [p_AIL_quick_ms_length] }
static FARPROC p_AIL_quick_ms_position;
D __declspec(naked) void __stdcall AIL_quick_ms_position(int a0) { __asm jmp dword ptr [p_AIL_quick_ms_position] }
static FARPROC p_AIL_quick_play;
D __declspec(naked) void __stdcall AIL_quick_play(int a0, int a1) { __asm jmp dword ptr [p_AIL_quick_play] }
static FARPROC p_AIL_quick_set_ms_position;
D __declspec(naked) void __stdcall AIL_quick_set_ms_position(int a0, int a1) { __asm jmp dword ptr [p_AIL_quick_set_ms_position] }
static FARPROC p_AIL_quick_set_reverb;
D __declspec(naked) void __stdcall AIL_quick_set_reverb(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_quick_set_reverb] }
static FARPROC p_AIL_quick_set_speed;
D __declspec(naked) void __stdcall AIL_quick_set_speed(int a0, int a1) { __asm jmp dword ptr [p_AIL_quick_set_speed] }
static FARPROC p_AIL_quick_set_volume;
D __declspec(naked) void __stdcall AIL_quick_set_volume(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_quick_set_volume] }
static FARPROC p_AIL_quick_shutdown;
D __declspec(naked) void __stdcall AIL_quick_shutdown(void) { __asm jmp dword ptr [p_AIL_quick_shutdown] }
static FARPROC p_AIL_quick_startup;
D __declspec(naked) void __stdcall AIL_quick_startup(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_quick_startup] }
static FARPROC p_AIL_quick_status;
D __declspec(naked) void __stdcall AIL_quick_status(int a0) { __asm jmp dword ptr [p_AIL_quick_status] }
static FARPROC p_AIL_quick_type;
D __declspec(naked) void __stdcall AIL_quick_type(int a0) { __asm jmp dword ptr [p_AIL_quick_type] }
static FARPROC p_AIL_quick_unload;
D __declspec(naked) void __stdcall AIL_quick_unload(int a0) { __asm jmp dword ptr [p_AIL_quick_unload] }
static FARPROC p_AIL_redbook_close;
D __declspec(naked) void __stdcall AIL_redbook_close(int a0) { __asm jmp dword ptr [p_AIL_redbook_close] }
static FARPROC p_AIL_redbook_eject;
D __declspec(naked) void __stdcall AIL_redbook_eject(int a0) { __asm jmp dword ptr [p_AIL_redbook_eject] }
static FARPROC p_AIL_redbook_id;
D __declspec(naked) void __stdcall AIL_redbook_id(int a0) { __asm jmp dword ptr [p_AIL_redbook_id] }
static FARPROC p_AIL_redbook_open;
D __declspec(naked) void __stdcall AIL_redbook_open(int a0) { __asm jmp dword ptr [p_AIL_redbook_open] }
static FARPROC p_AIL_redbook_open_drive;
D __declspec(naked) void __stdcall AIL_redbook_open_drive(int a0) { __asm jmp dword ptr [p_AIL_redbook_open_drive] }
static FARPROC p_AIL_redbook_pause;
D __declspec(naked) void __stdcall AIL_redbook_pause(int a0) { __asm jmp dword ptr [p_AIL_redbook_pause] }
static FARPROC p_AIL_redbook_play;
D __declspec(naked) void __stdcall AIL_redbook_play(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_redbook_play] }
static FARPROC p_AIL_redbook_position;
D __declspec(naked) void __stdcall AIL_redbook_position(int a0) { __asm jmp dword ptr [p_AIL_redbook_position] }
static FARPROC p_AIL_redbook_resume;
D __declspec(naked) void __stdcall AIL_redbook_resume(int a0) { __asm jmp dword ptr [p_AIL_redbook_resume] }
static FARPROC p_AIL_redbook_retract;
D __declspec(naked) void __stdcall AIL_redbook_retract(int a0) { __asm jmp dword ptr [p_AIL_redbook_retract] }
static FARPROC p_AIL_redbook_set_volume;
D __declspec(naked) void __stdcall AIL_redbook_set_volume(int a0, int a1) { __asm jmp dword ptr [p_AIL_redbook_set_volume] }
static FARPROC p_AIL_redbook_status;
D __declspec(naked) void __stdcall AIL_redbook_status(int a0) { __asm jmp dword ptr [p_AIL_redbook_status] }
static FARPROC p_AIL_redbook_stop;
D __declspec(naked) void __stdcall AIL_redbook_stop(int a0) { __asm jmp dword ptr [p_AIL_redbook_stop] }
static FARPROC p_AIL_redbook_track;
D __declspec(naked) void __stdcall AIL_redbook_track(int a0) { __asm jmp dword ptr [p_AIL_redbook_track] }
static FARPROC p_AIL_redbook_track_info;
D __declspec(naked) void __stdcall AIL_redbook_track_info(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_redbook_track_info] }
static FARPROC p_AIL_redbook_tracks;
D __declspec(naked) void __stdcall AIL_redbook_tracks(int a0) { __asm jmp dword ptr [p_AIL_redbook_tracks] }
static FARPROC p_AIL_redbook_volume;
D __declspec(naked) void __stdcall AIL_redbook_volume(int a0) { __asm jmp dword ptr [p_AIL_redbook_volume] }
static FARPROC p_AIL_register_3D_EOS_callback;
D __declspec(naked) void __stdcall AIL_register_3D_EOS_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_3D_EOS_callback] }
static FARPROC p_AIL_register_EOB_callback;
D __declspec(naked) void __stdcall AIL_register_EOB_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_EOB_callback] }
static FARPROC p_AIL_register_EOF_callback;
D __declspec(naked) void __stdcall AIL_register_EOF_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_EOF_callback] }
static FARPROC p_AIL_register_EOS_callback;
D __declspec(naked) void __stdcall AIL_register_EOS_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_EOS_callback] }
static FARPROC p_AIL_register_ICA_array;
D __declspec(naked) void __stdcall AIL_register_ICA_array(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_ICA_array] }
static FARPROC p_AIL_register_SOB_callback;
D __declspec(naked) void __stdcall AIL_register_SOB_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_SOB_callback] }
static FARPROC p_AIL_register_beat_callback;
D __declspec(naked) void __stdcall AIL_register_beat_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_beat_callback] }
static FARPROC p_AIL_register_event_callback;
D __declspec(naked) void __stdcall AIL_register_event_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_event_callback] }
static FARPROC p_AIL_register_prefix_callback;
D __declspec(naked) void __stdcall AIL_register_prefix_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_prefix_callback] }
static FARPROC p_AIL_register_sequence_callback;
D __declspec(naked) void __stdcall AIL_register_sequence_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_sequence_callback] }
static FARPROC p_AIL_register_stream_callback;
D __declspec(naked) void __stdcall AIL_register_stream_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_stream_callback] }
static FARPROC p_AIL_register_timbre_callback;
D __declspec(naked) void __stdcall AIL_register_timbre_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_timbre_callback] }
static FARPROC p_AIL_register_timer;
D __declspec(naked) void __stdcall AIL_register_timer(int a0) { __asm jmp dword ptr [p_AIL_register_timer] }
static FARPROC p_AIL_register_trigger_callback;
D __declspec(naked) void __stdcall AIL_register_trigger_callback(int a0, int a1) { __asm jmp dword ptr [p_AIL_register_trigger_callback] }
static FARPROC p_AIL_release_3D_sample_handle;
static FARPROC p_AIL_release_all_timers;
D __declspec(naked) void __stdcall AIL_release_all_timers(void) { __asm jmp dword ptr [p_AIL_release_all_timers] }
static FARPROC p_AIL_release_channel;
D __declspec(naked) void __stdcall AIL_release_channel(int a0, int a1) { __asm jmp dword ptr [p_AIL_release_channel] }
static FARPROC p_AIL_release_sample_handle;
static FARPROC p_AIL_release_sequence_handle;
D __declspec(naked) void __stdcall AIL_release_sequence_handle(int a0) { __asm jmp dword ptr [p_AIL_release_sequence_handle] }
static FARPROC p_AIL_release_timer_handle;
D __declspec(naked) void __stdcall AIL_release_timer_handle(int a0) { __asm jmp dword ptr [p_AIL_release_timer_handle] }
static FARPROC p_AIL_request_EOB_ASI_reset;
D __declspec(naked) void __stdcall AIL_request_EOB_ASI_reset(int a0, int a1) { __asm jmp dword ptr [p_AIL_request_EOB_ASI_reset] }
static FARPROC p_AIL_resume_3D_sample;
static FARPROC p_AIL_resume_sample;
static FARPROC p_AIL_resume_sequence;
D __declspec(naked) void __stdcall AIL_resume_sequence(int a0) { __asm jmp dword ptr [p_AIL_resume_sequence] }
static FARPROC p_AIL_sample_buffer_info;
D __declspec(naked) void __stdcall AIL_sample_buffer_info(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_sample_buffer_info] }
static FARPROC p_AIL_sample_buffer_ready;
D __declspec(naked) void __stdcall AIL_sample_buffer_ready(int a0) { __asm jmp dword ptr [p_AIL_sample_buffer_ready] }
static FARPROC p_AIL_sample_granularity;
D __declspec(naked) void __stdcall AIL_sample_granularity(int a0) { __asm jmp dword ptr [p_AIL_sample_granularity] }
static FARPROC p_AIL_sample_loop_count;
D __declspec(naked) void __stdcall AIL_sample_loop_count(int a0) { __asm jmp dword ptr [p_AIL_sample_loop_count] }
static FARPROC p_AIL_sample_ms_position;
D __declspec(naked) void __stdcall AIL_sample_ms_position(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_sample_ms_position] }
static FARPROC p_AIL_sample_pan;
D __declspec(naked) void __stdcall AIL_sample_pan(int a0) { __asm jmp dword ptr [p_AIL_sample_pan] }
static FARPROC p_AIL_sample_playback_rate;
D __declspec(naked) void __stdcall AIL_sample_playback_rate(int a0) { __asm jmp dword ptr [p_AIL_sample_playback_rate] }
static FARPROC p_AIL_sample_position;
D __declspec(naked) void __stdcall AIL_sample_position(int a0) { __asm jmp dword ptr [p_AIL_sample_position] }
static FARPROC p_AIL_sample_reverb;
D __declspec(naked) void __stdcall AIL_sample_reverb(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_sample_reverb] }
static FARPROC p_AIL_sample_status;
D __declspec(naked) void __stdcall AIL_sample_status(int a0) { __asm jmp dword ptr [p_AIL_sample_status] }
static FARPROC p_AIL_sample_user_data;
D __declspec(naked) void __stdcall AIL_sample_user_data(int a0, int a1) { __asm jmp dword ptr [p_AIL_sample_user_data] }
static FARPROC p_AIL_sample_volume;
D __declspec(naked) void __stdcall AIL_sample_volume(int a0) { __asm jmp dword ptr [p_AIL_sample_volume] }
static FARPROC p_AIL_send_channel_voice_message;
D __declspec(naked) void __stdcall AIL_send_channel_voice_message(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_send_channel_voice_message] }
static FARPROC p_AIL_send_sysex_message;
D __declspec(naked) void __stdcall AIL_send_sysex_message(int a0, int a1) { __asm jmp dword ptr [p_AIL_send_sysex_message] }
static FARPROC p_AIL_sequence_loop_count;
D __declspec(naked) void __stdcall AIL_sequence_loop_count(int a0) { __asm jmp dword ptr [p_AIL_sequence_loop_count] }
static FARPROC p_AIL_sequence_ms_position;
D __declspec(naked) void __stdcall AIL_sequence_ms_position(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_sequence_ms_position] }
static FARPROC p_AIL_sequence_position;
D __declspec(naked) void __stdcall AIL_sequence_position(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_sequence_position] }
static FARPROC p_AIL_sequence_status;
D __declspec(naked) void __stdcall AIL_sequence_status(int a0) { __asm jmp dword ptr [p_AIL_sequence_status] }
static FARPROC p_AIL_sequence_tempo;
D __declspec(naked) void __stdcall AIL_sequence_tempo(int a0) { __asm jmp dword ptr [p_AIL_sequence_tempo] }
static FARPROC p_AIL_sequence_user_data;
D __declspec(naked) void __stdcall AIL_sequence_user_data(int a0, int a1) { __asm jmp dword ptr [p_AIL_sequence_user_data] }
static FARPROC p_AIL_sequence_volume;
D __declspec(naked) void __stdcall AIL_sequence_volume(int a0) { __asm jmp dword ptr [p_AIL_sequence_volume] }
static FARPROC p_AIL_serve;
D __declspec(naked) void __stdcall AIL_serve(void) { __asm jmp dword ptr [p_AIL_serve] }
static FARPROC p_AIL_service_stream;
D __declspec(naked) void __stdcall AIL_service_stream(int a0, int a1) { __asm jmp dword ptr [p_AIL_service_stream] }
static FARPROC p_AIL_set_3D_orientation;
static FARPROC p_AIL_set_3D_position;
static FARPROC p_AIL_set_3D_provider_preference;
D __declspec(naked) void __stdcall AIL_set_3D_provider_preference(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_3D_provider_preference] }
static FARPROC p_AIL_set_3D_room_type;
static FARPROC p_AIL_set_3D_sample_cone;
static FARPROC p_AIL_set_3D_sample_distances;
static FARPROC p_AIL_set_3D_sample_effects_level;
static FARPROC p_AIL_set_3D_sample_file;
static FARPROC p_AIL_set_3D_sample_info;
D __declspec(naked) void __stdcall AIL_set_3D_sample_info(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_3D_sample_info] }
static FARPROC p_AIL_set_3D_sample_loop_block;
D __declspec(naked) void __stdcall AIL_set_3D_sample_loop_block(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_3D_sample_loop_block] }
static FARPROC p_AIL_set_3D_sample_loop_count;
static FARPROC p_AIL_set_3D_sample_obstruction;
D __declspec(naked) void __stdcall AIL_set_3D_sample_obstruction(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_3D_sample_obstruction] }
static FARPROC p_AIL_set_3D_sample_occlusion;
D __declspec(naked) void __stdcall AIL_set_3D_sample_occlusion(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_3D_sample_occlusion] }
static FARPROC p_AIL_set_3D_sample_offset;
static FARPROC p_AIL_set_3D_sample_playback_rate;
static FARPROC p_AIL_set_3D_sample_preference;
D __declspec(naked) void __stdcall AIL_set_3D_sample_preference(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_3D_sample_preference] }
static FARPROC p_AIL_set_3D_sample_volume;
static FARPROC p_AIL_set_3D_speaker_type;
static FARPROC p_AIL_set_3D_user_data;
D __declspec(naked) void __stdcall AIL_set_3D_user_data(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_3D_user_data] }
static FARPROC p_AIL_set_3D_velocity;
static FARPROC p_AIL_set_3D_velocity_vector;
static FARPROC p_AIL_set_DLS_processor;
D __declspec(naked) void __stdcall AIL_set_DLS_processor(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_DLS_processor] }
static FARPROC p_AIL_set_DirectSound_HWND;
D __declspec(naked) void __stdcall AIL_set_DirectSound_HWND(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_DirectSound_HWND] }
static FARPROC p_AIL_set_XMIDI_master_volume;
D __declspec(naked) void __stdcall AIL_set_XMIDI_master_volume(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_XMIDI_master_volume] }
static FARPROC p_AIL_set_digital_driver_processor;
D __declspec(naked) void __stdcall AIL_set_digital_driver_processor(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_digital_driver_processor] }
static FARPROC p_AIL_set_digital_master_volume;
static FARPROC p_AIL_set_error;
D __declspec(naked) void __stdcall AIL_set_error(int a0) { __asm jmp dword ptr [p_AIL_set_error] }
static FARPROC p_AIL_set_file_async_callbacks;
D __declspec(naked) void __stdcall AIL_set_file_async_callbacks(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_set_file_async_callbacks] }
static FARPROC p_AIL_set_file_callbacks;
D __declspec(naked) void __stdcall AIL_set_file_callbacks(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_set_file_callbacks] }
static FARPROC p_AIL_set_filter_DLS_preference;
D __declspec(naked) void __stdcall AIL_set_filter_DLS_preference(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_filter_DLS_preference] }
static FARPROC p_AIL_set_filter_preference;
D __declspec(naked) void __stdcall AIL_set_filter_preference(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_filter_preference] }
static FARPROC p_AIL_set_filter_sample_preference;
static FARPROC p_AIL_set_filter_stream_preference;
D __declspec(naked) void __stdcall AIL_set_filter_stream_preference(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_filter_stream_preference] }
static FARPROC p_AIL_set_input_state;
D __declspec(naked) void __stdcall AIL_set_input_state(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_input_state] }
static FARPROC p_AIL_set_named_sample_file;
static FARPROC p_AIL_set_preference;
static FARPROC p_AIL_set_redist_directory;
D __declspec(naked) void __stdcall AIL_set_redist_directory(int a0) { __asm jmp dword ptr [p_AIL_set_redist_directory] }
static FARPROC p_AIL_set_sample_address;
D __declspec(naked) void __stdcall AIL_set_sample_address(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_sample_address] }
static FARPROC p_AIL_set_sample_adpcm_block_size;
D __declspec(naked) void __stdcall AIL_set_sample_adpcm_block_size(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_sample_adpcm_block_size] }
static FARPROC p_AIL_set_sample_file;
static FARPROC p_AIL_set_sample_loop_block;
D __declspec(naked) void __stdcall AIL_set_sample_loop_block(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_sample_loop_block] }
static FARPROC p_AIL_set_sample_loop_count;
static FARPROC p_AIL_set_sample_ms_position;
static FARPROC p_AIL_set_sample_pan;
static FARPROC p_AIL_set_sample_playback_rate;
static FARPROC p_AIL_set_sample_position;
static FARPROC p_AIL_set_sample_processor;
static FARPROC p_AIL_set_sample_reverb;
D __declspec(naked) void __stdcall AIL_set_sample_reverb(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_set_sample_reverb] }
static FARPROC p_AIL_set_sample_type;
D __declspec(naked) void __stdcall AIL_set_sample_type(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_sample_type] }
static FARPROC p_AIL_set_sample_user_data;
D __declspec(naked) void __stdcall AIL_set_sample_user_data(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_sample_user_data] }
static FARPROC p_AIL_set_sample_volume;
static FARPROC p_AIL_set_sequence_loop_count;
D __declspec(naked) void __stdcall AIL_set_sequence_loop_count(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_sequence_loop_count] }
static FARPROC p_AIL_set_sequence_ms_position;
D __declspec(naked) void __stdcall AIL_set_sequence_ms_position(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_sequence_ms_position] }
static FARPROC p_AIL_set_sequence_tempo;
D __declspec(naked) void __stdcall AIL_set_sequence_tempo(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_sequence_tempo] }
static FARPROC p_AIL_set_sequence_user_data;
D __declspec(naked) void __stdcall AIL_set_sequence_user_data(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_sequence_user_data] }
static FARPROC p_AIL_set_sequence_volume;
D __declspec(naked) void __stdcall AIL_set_sequence_volume(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_sequence_volume] }
static FARPROC p_AIL_set_stream_loop_block;
static FARPROC p_AIL_set_stream_loop_count;
static FARPROC p_AIL_set_stream_ms_position;
static FARPROC p_AIL_set_stream_pan;
static FARPROC p_AIL_set_stream_playback_rate;
static FARPROC p_AIL_set_stream_position;
static FARPROC p_AIL_set_stream_processor;
D __declspec(naked) void __stdcall AIL_set_stream_processor(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_stream_processor] }
static FARPROC p_AIL_set_stream_reverb;
D __declspec(naked) void __stdcall AIL_set_stream_reverb(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_set_stream_reverb] }
static FARPROC p_AIL_set_stream_user_data;
D __declspec(naked) void __stdcall AIL_set_stream_user_data(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_set_stream_user_data] }
static FARPROC p_AIL_set_stream_volume;
static FARPROC p_AIL_set_timer_divisor;
D __declspec(naked) void __stdcall AIL_set_timer_divisor(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_timer_divisor] }
static FARPROC p_AIL_set_timer_frequency;
D __declspec(naked) void __stdcall AIL_set_timer_frequency(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_timer_frequency] }
static FARPROC p_AIL_set_timer_period;
D __declspec(naked) void __stdcall AIL_set_timer_period(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_timer_period] }
static FARPROC p_AIL_set_timer_user;
D __declspec(naked) void __stdcall AIL_set_timer_user(int a0, int a1) { __asm jmp dword ptr [p_AIL_set_timer_user] }
static FARPROC p_AIL_shutdown;
static FARPROC p_AIL_size_processed_digital_audio;
D __declspec(naked) void __stdcall AIL_size_processed_digital_audio(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_size_processed_digital_audio] }
static FARPROC p_AIL_sprintf;
D __declspec(naked) void __cdecl AIL_sprintf(void) { __asm jmp dword ptr [p_AIL_sprintf] }
static FARPROC p_AIL_start_3D_sample;
static FARPROC p_AIL_start_all_timers;
D __declspec(naked) void __stdcall AIL_start_all_timers(void) { __asm jmp dword ptr [p_AIL_start_all_timers] }
static FARPROC p_AIL_start_sample;
static FARPROC p_AIL_start_sequence;
D __declspec(naked) void __stdcall AIL_start_sequence(int a0) { __asm jmp dword ptr [p_AIL_start_sequence] }
static FARPROC p_AIL_start_stream;
static FARPROC p_AIL_start_timer;
D __declspec(naked) void __stdcall AIL_start_timer(int a0) { __asm jmp dword ptr [p_AIL_start_timer] }
static FARPROC p_AIL_startup;
static FARPROC p_AIL_stop_3D_sample;
static FARPROC p_AIL_stop_all_timers;
D __declspec(naked) void __stdcall AIL_stop_all_timers(void) { __asm jmp dword ptr [p_AIL_stop_all_timers] }
static FARPROC p_AIL_stop_sample;
static FARPROC p_AIL_stop_sequence;
D __declspec(naked) void __stdcall AIL_stop_sequence(int a0) { __asm jmp dword ptr [p_AIL_stop_sequence] }
static FARPROC p_AIL_stop_timer;
D __declspec(naked) void __stdcall AIL_stop_timer(int a0) { __asm jmp dword ptr [p_AIL_stop_timer] }
static FARPROC p_AIL_stream_info;
D __declspec(naked) void __stdcall AIL_stream_info(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_AIL_stream_info] }
static FARPROC p_AIL_stream_loop_count;
D __declspec(naked) void __stdcall AIL_stream_loop_count(int a0) { __asm jmp dword ptr [p_AIL_stream_loop_count] }
static FARPROC p_AIL_stream_ms_position;
D __declspec(naked) void __stdcall AIL_stream_ms_position(int a0, int a1, int a2) { __asm jmp dword ptr [p_AIL_stream_ms_position] }
static FARPROC p_AIL_stream_pan;
D __declspec(naked) void __stdcall AIL_stream_pan(int a0) { __asm jmp dword ptr [p_AIL_stream_pan] }
static FARPROC p_AIL_stream_playback_rate;
D __declspec(naked) void __stdcall AIL_stream_playback_rate(int a0) { __asm jmp dword ptr [p_AIL_stream_playback_rate] }
static FARPROC p_AIL_stream_position;
D __declspec(naked) void __stdcall AIL_stream_position(int a0) { __asm jmp dword ptr [p_AIL_stream_position] }
static FARPROC p_AIL_stream_reverb;
D __declspec(naked) void __stdcall AIL_stream_reverb(int a0, int a1, int a2, int a3) { __asm jmp dword ptr [p_AIL_stream_reverb] }
static FARPROC p_AIL_stream_status;
D __declspec(naked) void __stdcall AIL_stream_status(int a0) { __asm jmp dword ptr [p_AIL_stream_status] }
static FARPROC p_AIL_stream_user_data;
D __declspec(naked) void __stdcall AIL_stream_user_data(int a0, int a1) { __asm jmp dword ptr [p_AIL_stream_user_data] }
static FARPROC p_AIL_stream_volume;
D __declspec(naked) void __stdcall AIL_stream_volume(int a0) { __asm jmp dword ptr [p_AIL_stream_volume] }
static FARPROC p_AIL_true_sequence_channel;
D __declspec(naked) void __stdcall AIL_true_sequence_channel(int a0, int a1) { __asm jmp dword ptr [p_AIL_true_sequence_channel] }
static FARPROC p_AIL_unlock;
D __declspec(naked) void __stdcall AIL_unlock(void) { __asm jmp dword ptr [p_AIL_unlock] }
static FARPROC p_AIL_unlock_mutex;
D __declspec(naked) void __stdcall AIL_unlock_mutex(void) { __asm jmp dword ptr [p_AIL_unlock_mutex] }
static FARPROC p_AIL_update_3D_position;
D __declspec(naked) void __stdcall AIL_update_3D_position(int a0, int a1) { __asm jmp dword ptr [p_AIL_update_3D_position] }
static FARPROC p_AIL_us_count;
D __declspec(naked) void __stdcall AIL_us_count(void) { __asm jmp dword ptr [p_AIL_us_count] }
static FARPROC p_AIL_waveOutClose;
static FARPROC p_AIL_waveOutOpen;
static FARPROC p_DLSClose;
D __declspec(naked) void __cdecl DLSClose(void) { __asm jmp dword ptr [p_DLSClose] }
static FARPROC p_DLSCompactMemory;
D __declspec(naked) void __cdecl DLSCompactMemory(void) { __asm jmp dword ptr [p_DLSCompactMemory] }
static FARPROC p_DLSGetInfo;
D __declspec(naked) void __cdecl DLSGetInfo(void) { __asm jmp dword ptr [p_DLSGetInfo] }
static FARPROC p_DLSLoadFile;
D __declspec(naked) void __cdecl DLSLoadFile(void) { __asm jmp dword ptr [p_DLSLoadFile] }
static FARPROC p_DLSLoadMemFile;
D __declspec(naked) void __cdecl DLSLoadMemFile(void) { __asm jmp dword ptr [p_DLSLoadMemFile] }
static FARPROC p_DLSMSSGetCPU;
D __declspec(naked) void __stdcall DLSMSSGetCPU(int a0) { __asm jmp dword ptr [p_DLSMSSGetCPU] }
static FARPROC p_DLSMSSOpen;
D __declspec(naked) void __cdecl DLSMSSOpen(void) { __asm jmp dword ptr [p_DLSMSSOpen] }
static FARPROC p_DLSSetAttribute;
D __declspec(naked) void __cdecl DLSSetAttribute(void) { __asm jmp dword ptr [p_DLSSetAttribute] }
static FARPROC p_DLSUnloadAll;
D __declspec(naked) void __cdecl DLSUnloadAll(void) { __asm jmp dword ptr [p_DLSUnloadAll] }
static FARPROC p_DLSUnloadFile;
D __declspec(naked) void __cdecl DLSUnloadFile(void) { __asm jmp dword ptr [p_DLSUnloadFile] }
static FARPROC p_DllMain;
static FARPROC p_RIB_alloc_provider_handle;
D __declspec(naked) void __cdecl RIB_alloc_provider_handle(void) { __asm jmp dword ptr [p_RIB_alloc_provider_handle] }
static FARPROC p_RIB_enumerate_interface;
D __declspec(naked) void __cdecl RIB_enumerate_interface(void) { __asm jmp dword ptr [p_RIB_enumerate_interface] }
static FARPROC p_RIB_enumerate_providers;
D __declspec(naked) void __stdcall RIB_enumerate_providers(int a0, int a1, int a2) { __asm jmp dword ptr [p_RIB_enumerate_providers] }
static FARPROC p_RIB_error;
D __declspec(naked) void __cdecl RIB_error(void) { __asm jmp dword ptr [p_RIB_error] }
static FARPROC p_RIB_find_file_dec_provider;
D __declspec(naked) void __stdcall RIB_find_file_dec_provider(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_RIB_find_file_dec_provider] }
static FARPROC p_RIB_find_file_provider;
D __declspec(naked) void __cdecl RIB_find_file_provider(void) { __asm jmp dword ptr [p_RIB_find_file_provider] }
static FARPROC p_RIB_find_files_provider;
D __declspec(naked) void __stdcall RIB_find_files_provider(int a0, int a1, int a2, int a3, int a4) { __asm jmp dword ptr [p_RIB_find_files_provider] }
static FARPROC p_RIB_find_provider;
D __declspec(naked) void __stdcall RIB_find_provider(int a0, int a1, int a2) { __asm jmp dword ptr [p_RIB_find_provider] }
static FARPROC p_RIB_free_provider_handle;
D __declspec(naked) void __cdecl RIB_free_provider_handle(void) { __asm jmp dword ptr [p_RIB_free_provider_handle] }
static FARPROC p_RIB_free_provider_library;
D __declspec(naked) void __cdecl RIB_free_provider_library(void) { __asm jmp dword ptr [p_RIB_free_provider_library] }
static FARPROC p_RIB_load_application_providers;
D __declspec(naked) void __stdcall RIB_load_application_providers(int a0) { __asm jmp dword ptr [p_RIB_load_application_providers] }
static FARPROC p_RIB_load_provider_library;
D __declspec(naked) void __cdecl RIB_load_provider_library(void) { __asm jmp dword ptr [p_RIB_load_provider_library] }
static FARPROC p_RIB_provider_library_handle;
D __declspec(naked) void __stdcall RIB_provider_library_handle(void) { __asm jmp dword ptr [p_RIB_provider_library_handle] }
static FARPROC p_RIB_provider_system_data;
D __declspec(naked) void __stdcall RIB_provider_system_data(int a0, int a1) { __asm jmp dword ptr [p_RIB_provider_system_data] }
static FARPROC p_RIB_provider_user_data;
D __declspec(naked) void __stdcall RIB_provider_user_data(int a0, int a1) { __asm jmp dword ptr [p_RIB_provider_user_data] }
static FARPROC p_RIB_register_interface;
D __declspec(naked) void __cdecl RIB_register_interface(void) { __asm jmp dword ptr [p_RIB_register_interface] }
static FARPROC p_RIB_request_interface;
D __declspec(naked) void __cdecl RIB_request_interface(void) { __asm jmp dword ptr [p_RIB_request_interface] }
static FARPROC p_RIB_request_interface_entry;
D __declspec(naked) void __cdecl RIB_request_interface_entry(void) { __asm jmp dword ptr [p_RIB_request_interface_entry] }
static FARPROC p_RIB_set_provider_system_data;
D __declspec(naked) void __stdcall RIB_set_provider_system_data(int a0, int a1, int a2) { __asm jmp dword ptr [p_RIB_set_provider_system_data] }
static FARPROC p_RIB_set_provider_user_data;
D __declspec(naked) void __stdcall RIB_set_provider_user_data(int a0, int a1, int a2) { __asm jmp dword ptr [p_RIB_set_provider_user_data] }
static FARPROC p_RIB_type_string;
D __declspec(naked) void __cdecl RIB_type_string(void) { __asm jmp dword ptr [p_RIB_type_string] }
static FARPROC p_RIB_unregister_interface;
D __declspec(naked) void __cdecl RIB_unregister_interface(void) { __asm jmp dword ptr [p_RIB_unregister_interface] }
static const struct { const char *name; FARPROC *p; } Table[] = {
    {"@stream_background@0", &p_stream_background},
    {"_AIL_3D_orientation@28", &p_AIL_3D_orientation},
    {"_AIL_3D_position@16", &p_AIL_3D_position},
    {"_AIL_3D_provider_attribute@12", &p_AIL_3D_provider_attribute},
    {"_AIL_3D_room_type@4", &p_AIL_3D_room_type},
    {"_AIL_3D_sample_attribute@12", &p_AIL_3D_sample_attribute},
    {"_AIL_3D_sample_cone@16", &p_AIL_3D_sample_cone},
    {"_AIL_3D_sample_distances@12", &p_AIL_3D_sample_distances},
    {"_AIL_3D_sample_effects_level@4", &p_AIL_3D_sample_effects_level},
    {"_AIL_3D_sample_length@4", &p_AIL_3D_sample_length},
    {"_AIL_3D_sample_loop_count@4", &p_AIL_3D_sample_loop_count},
    {"_AIL_3D_sample_obstruction@4", &p_AIL_3D_sample_obstruction},
    {"_AIL_3D_sample_occlusion@4", &p_AIL_3D_sample_occlusion},
    {"_AIL_3D_sample_offset@4", &p_AIL_3D_sample_offset},
    {"_AIL_3D_sample_playback_rate@4", &p_AIL_3D_sample_playback_rate},
    {"_AIL_3D_sample_status@4", &p_AIL_3D_sample_status},
    {"_AIL_3D_sample_volume@4", &p_AIL_3D_sample_volume},
    {"_AIL_3D_speaker_type@4", &p_AIL_3D_speaker_type},
    {"_AIL_3D_user_data@8", &p_AIL_3D_user_data},
    {"_AIL_3D_velocity@16", &p_AIL_3D_velocity},
    {"_AIL_DLS_close@8", &p_AIL_DLS_close},
    {"_AIL_DLS_compact@4", &p_AIL_DLS_compact},
    {"_AIL_DLS_get_info@12", &p_AIL_DLS_get_info},
    {"_AIL_DLS_get_reverb@16", &p_AIL_DLS_get_reverb},
    {"_AIL_DLS_load_file@12", &p_AIL_DLS_load_file},
    {"_AIL_DLS_load_memory@12", &p_AIL_DLS_load_memory},
    {"_AIL_DLS_open@28", &p_AIL_DLS_open},
    {"_AIL_DLS_set_reverb@16", &p_AIL_DLS_set_reverb},
    {"_AIL_DLS_unload@8", &p_AIL_DLS_unload},
    {"_AIL_HWND@0", &p_AIL_HWND},
    {"_AIL_MIDI_handle_reacquire@4", &p_AIL_MIDI_handle_reacquire},
    {"_AIL_MIDI_handle_release@4", &p_AIL_MIDI_handle_release},
    {"_AIL_MIDI_to_XMI@20", &p_AIL_MIDI_to_XMI},
    {"_AIL_MMX_available@0", &p_AIL_MMX_available},
    {"_AIL_WAV_file_write@20", &p_AIL_WAV_file_write},
    {"_AIL_WAV_info@8", &p_AIL_WAV_info},
    {"_AIL_XMIDI_master_volume@4", &p_AIL_XMIDI_master_volume},
    {"_AIL_active_3D_sample_count@4", &p_AIL_active_3D_sample_count},
    {"_AIL_active_sample_count@4", &p_AIL_active_sample_count},
    {"_AIL_active_sequence_count@4", &p_AIL_active_sequence_count},
    {"_AIL_allocate_3D_sample_handle@4", &p_AIL_allocate_3D_sample_handle},
    {"_AIL_allocate_file_sample@12", &p_AIL_allocate_file_sample},
    {"_AIL_allocate_sample_handle@4", &p_AIL_allocate_sample_handle},
    {"_AIL_allocate_sequence_handle@4", &p_AIL_allocate_sequence_handle},
    {"_AIL_auto_service_stream@8", &p_AIL_auto_service_stream},
    {"_AIL_auto_update_3D_position@8", &p_AIL_auto_update_3D_position},
    {"_AIL_background@0", &p_AIL_background},
    {"_AIL_branch_index@8", &p_AIL_branch_index},
    {"_AIL_channel_notes@8", &p_AIL_channel_notes},
    {"_AIL_close_3D_listener@4", &p_AIL_close_3D_listener},
    {"_AIL_close_3D_object@4", &p_AIL_close_3D_object},
    {"_AIL_close_3D_provider@4", &p_AIL_close_3D_provider},
    {"_AIL_close_XMIDI_driver@4", &p_AIL_close_XMIDI_driver},
    {"_AIL_close_digital_driver@4", &p_AIL_close_digital_driver},
    {"_AIL_close_filter@4", &p_AIL_close_filter},
    {"_AIL_close_input@4", &p_AIL_close_input},
    {"_AIL_close_stream@4", &p_AIL_close_stream},
    {"_AIL_compress_ADPCM@12", &p_AIL_compress_ADPCM},
    {"_AIL_compress_ASI@20", &p_AIL_compress_ASI},
    {"_AIL_compress_DLS@20", &p_AIL_compress_DLS},
    {"_AIL_controller_value@12", &p_AIL_controller_value},
    {"_AIL_create_wave_synthesizer@16", &p_AIL_create_wave_synthesizer},
    {"AIL_debug_printf", &p_AIL_debug_printf},
    {"_AIL_decompress_ADPCM@12", &p_AIL_decompress_ADPCM},
    {"_AIL_decompress_ASI@24", &p_AIL_decompress_ASI},
    {"_AIL_delay@4", &p_AIL_delay},
    {"_AIL_destroy_wave_synthesizer@4", &p_AIL_destroy_wave_synthesizer},
    {"_AIL_digital_CPU_percent@4", &p_AIL_digital_CPU_percent},
    {"_AIL_digital_configuration@16", &p_AIL_digital_configuration},
    {"_AIL_digital_handle_reacquire@4", &p_AIL_digital_handle_reacquire},
    {"_AIL_digital_handle_release@4", &p_AIL_digital_handle_release},
    {"_AIL_digital_latency@4", &p_AIL_digital_latency},
    {"_AIL_digital_master_volume@4", &p_AIL_digital_master_volume},
    {"_AIL_end_3D_sample@4", &p_AIL_end_3D_sample},
    {"_AIL_end_sample@4", &p_AIL_end_sample},
    {"_AIL_end_sequence@4", &p_AIL_end_sequence},
    {"_AIL_enumerate_3D_provider_attributes@12", &p_AIL_enumerate_3D_provider_attributes},
    {"_AIL_enumerate_3D_providers@12", &p_AIL_enumerate_3D_providers},
    {"_AIL_enumerate_3D_sample_attributes@12", &p_AIL_enumerate_3D_sample_attributes},
    {"_AIL_enumerate_filter_attributes@12", &p_AIL_enumerate_filter_attributes},
    {"_AIL_enumerate_filter_sample_attributes@12", &p_AIL_enumerate_filter_sample_attributes},
    {"_AIL_enumerate_filters@12", &p_AIL_enumerate_filters},
    {"_AIL_extract_DLS@28", &p_AIL_extract_DLS},
    {"_AIL_file_error@0", &p_AIL_file_error},
    {"_AIL_file_read@8", &p_AIL_file_read},
    {"_AIL_file_size@4", &p_AIL_file_size},
    {"_AIL_file_type@8", &p_AIL_file_type},
    {"_AIL_file_write@12", &p_AIL_file_write},
    {"_AIL_filter_DLS_attribute@12", &p_AIL_filter_DLS_attribute},
    {"_AIL_filter_DLS_with_XMI@24", &p_AIL_filter_DLS_with_XMI},
    {"_AIL_filter_attribute@12", &p_AIL_filter_attribute},
    {"_AIL_filter_sample_attribute@12", &p_AIL_filter_sample_attribute},
    {"_AIL_filter_stream_attribute@12", &p_AIL_filter_stream_attribute},
    {"_AIL_find_DLS@24", &p_AIL_find_DLS},
    {"_AIL_get_DirectSound_info@12", &p_AIL_get_DirectSound_info},
    {"_AIL_get_input_info@4", &p_AIL_get_input_info},
    {"_AIL_get_preference@4", &p_AIL_get_preference},
    {"_AIL_get_timer_highest_delay@0", &p_AIL_get_timer_highest_delay},
    {"_AIL_init_sample@4", &p_AIL_init_sample},
    {"_AIL_init_sequence@12", &p_AIL_init_sequence},
    {"_AIL_last_error@0", &p_AIL_last_error},
    {"_AIL_list_DLS@20", &p_AIL_list_DLS},
    {"_AIL_list_MIDI@20", &p_AIL_list_MIDI},
    {"_AIL_load_sample_buffer@16", &p_AIL_load_sample_buffer},
    {"_AIL_lock@0", &p_AIL_lock},
    {"_AIL_lock_channel@4", &p_AIL_lock_channel},
    {"_AIL_lock_mutex@0", &p_AIL_lock_mutex},
    {"_AIL_map_sequence_channel@12", &p_AIL_map_sequence_channel},
    {"_AIL_mem_alloc_lock@4", &p_AIL_mem_alloc_lock},
    {"_AIL_mem_free_lock@4", &p_AIL_mem_free_lock},
    {"_AIL_merge_DLS_with_XMI@16", &p_AIL_merge_DLS_with_XMI},
    {"_AIL_midiOutClose@4", &p_AIL_midiOutClose},
    {"_AIL_midiOutOpen@12", &p_AIL_midiOutOpen},
    {"_AIL_minimum_sample_buffer_size@12", &p_AIL_minimum_sample_buffer_size},
    {"_AIL_ms_count@0", &p_AIL_ms_count},
    {"_AIL_open_3D_listener@4", &p_AIL_open_3D_listener},
    {"_AIL_open_3D_object@4", &p_AIL_open_3D_object},
    {"_AIL_open_3D_provider@4", &p_AIL_open_3D_provider},
    {"_AIL_open_XMIDI_driver@4", &p_AIL_open_XMIDI_driver},
    {"_AIL_open_digital_driver@16", &p_AIL_open_digital_driver},
    {"_AIL_open_filter@8", &p_AIL_open_filter},
    {"_AIL_open_input@4", &p_AIL_open_input},
    {"_AIL_open_stream@12", &p_AIL_open_stream},
    {"_AIL_open_stream_by_sample@16", &p_AIL_open_stream_by_sample},
    {"_AIL_pause_stream@8", &p_AIL_pause_stream},
    {"_AIL_primary_digital_driver@4", &p_AIL_primary_digital_driver},
    {"_AIL_process_digital_audio@24", &p_AIL_process_digital_audio},
    {"_AIL_quick_copy@4", &p_AIL_quick_copy},
    {"_AIL_quick_halt@4", &p_AIL_quick_halt},
    {"_AIL_quick_handles@12", &p_AIL_quick_handles},
    {"_AIL_quick_load@4", &p_AIL_quick_load},
    {"_AIL_quick_load_and_play@12", &p_AIL_quick_load_and_play},
    {"_AIL_quick_load_mem@8", &p_AIL_quick_load_mem},
    {"_AIL_quick_ms_length@4", &p_AIL_quick_ms_length},
    {"_AIL_quick_ms_position@4", &p_AIL_quick_ms_position},
    {"_AIL_quick_play@8", &p_AIL_quick_play},
    {"_AIL_quick_set_ms_position@8", &p_AIL_quick_set_ms_position},
    {"_AIL_quick_set_reverb@16", &p_AIL_quick_set_reverb},
    {"_AIL_quick_set_speed@8", &p_AIL_quick_set_speed},
    {"_AIL_quick_set_volume@12", &p_AIL_quick_set_volume},
    {"_AIL_quick_shutdown@0", &p_AIL_quick_shutdown},
    {"_AIL_quick_startup@20", &p_AIL_quick_startup},
    {"_AIL_quick_status@4", &p_AIL_quick_status},
    {"_AIL_quick_type@4", &p_AIL_quick_type},
    {"_AIL_quick_unload@4", &p_AIL_quick_unload},
    {"_AIL_redbook_close@4", &p_AIL_redbook_close},
    {"_AIL_redbook_eject@4", &p_AIL_redbook_eject},
    {"_AIL_redbook_id@4", &p_AIL_redbook_id},
    {"_AIL_redbook_open@4", &p_AIL_redbook_open},
    {"_AIL_redbook_open_drive@4", &p_AIL_redbook_open_drive},
    {"_AIL_redbook_pause@4", &p_AIL_redbook_pause},
    {"_AIL_redbook_play@12", &p_AIL_redbook_play},
    {"_AIL_redbook_position@4", &p_AIL_redbook_position},
    {"_AIL_redbook_resume@4", &p_AIL_redbook_resume},
    {"_AIL_redbook_retract@4", &p_AIL_redbook_retract},
    {"_AIL_redbook_set_volume@8", &p_AIL_redbook_set_volume},
    {"_AIL_redbook_status@4", &p_AIL_redbook_status},
    {"_AIL_redbook_stop@4", &p_AIL_redbook_stop},
    {"_AIL_redbook_track@4", &p_AIL_redbook_track},
    {"_AIL_redbook_track_info@16", &p_AIL_redbook_track_info},
    {"_AIL_redbook_tracks@4", &p_AIL_redbook_tracks},
    {"_AIL_redbook_volume@4", &p_AIL_redbook_volume},
    {"_AIL_register_3D_EOS_callback@8", &p_AIL_register_3D_EOS_callback},
    {"_AIL_register_EOB_callback@8", &p_AIL_register_EOB_callback},
    {"_AIL_register_EOF_callback@8", &p_AIL_register_EOF_callback},
    {"_AIL_register_EOS_callback@8", &p_AIL_register_EOS_callback},
    {"_AIL_register_ICA_array@8", &p_AIL_register_ICA_array},
    {"_AIL_register_SOB_callback@8", &p_AIL_register_SOB_callback},
    {"_AIL_register_beat_callback@8", &p_AIL_register_beat_callback},
    {"_AIL_register_event_callback@8", &p_AIL_register_event_callback},
    {"_AIL_register_prefix_callback@8", &p_AIL_register_prefix_callback},
    {"_AIL_register_sequence_callback@8", &p_AIL_register_sequence_callback},
    {"_AIL_register_stream_callback@8", &p_AIL_register_stream_callback},
    {"_AIL_register_timbre_callback@8", &p_AIL_register_timbre_callback},
    {"_AIL_register_timer@4", &p_AIL_register_timer},
    {"_AIL_register_trigger_callback@8", &p_AIL_register_trigger_callback},
    {"_AIL_release_3D_sample_handle@4", &p_AIL_release_3D_sample_handle},
    {"_AIL_release_all_timers@0", &p_AIL_release_all_timers},
    {"_AIL_release_channel@8", &p_AIL_release_channel},
    {"_AIL_release_sample_handle@4", &p_AIL_release_sample_handle},
    {"_AIL_release_sequence_handle@4", &p_AIL_release_sequence_handle},
    {"_AIL_release_timer_handle@4", &p_AIL_release_timer_handle},
    {"_AIL_request_EOB_ASI_reset@8", &p_AIL_request_EOB_ASI_reset},
    {"_AIL_resume_3D_sample@4", &p_AIL_resume_3D_sample},
    {"_AIL_resume_sample@4", &p_AIL_resume_sample},
    {"_AIL_resume_sequence@4", &p_AIL_resume_sequence},
    {"_AIL_sample_buffer_info@20", &p_AIL_sample_buffer_info},
    {"_AIL_sample_buffer_ready@4", &p_AIL_sample_buffer_ready},
    {"_AIL_sample_granularity@4", &p_AIL_sample_granularity},
    {"_AIL_sample_loop_count@4", &p_AIL_sample_loop_count},
    {"_AIL_sample_ms_position@12", &p_AIL_sample_ms_position},
    {"_AIL_sample_pan@4", &p_AIL_sample_pan},
    {"_AIL_sample_playback_rate@4", &p_AIL_sample_playback_rate},
    {"_AIL_sample_position@4", &p_AIL_sample_position},
    {"_AIL_sample_reverb@16", &p_AIL_sample_reverb},
    {"_AIL_sample_status@4", &p_AIL_sample_status},
    {"_AIL_sample_user_data@8", &p_AIL_sample_user_data},
    {"_AIL_sample_volume@4", &p_AIL_sample_volume},
    {"_AIL_send_channel_voice_message@20", &p_AIL_send_channel_voice_message},
    {"_AIL_send_sysex_message@8", &p_AIL_send_sysex_message},
    {"_AIL_sequence_loop_count@4", &p_AIL_sequence_loop_count},
    {"_AIL_sequence_ms_position@12", &p_AIL_sequence_ms_position},
    {"_AIL_sequence_position@12", &p_AIL_sequence_position},
    {"_AIL_sequence_status@4", &p_AIL_sequence_status},
    {"_AIL_sequence_tempo@4", &p_AIL_sequence_tempo},
    {"_AIL_sequence_user_data@8", &p_AIL_sequence_user_data},
    {"_AIL_sequence_volume@4", &p_AIL_sequence_volume},
    {"_AIL_serve@0", &p_AIL_serve},
    {"_AIL_service_stream@8", &p_AIL_service_stream},
    {"_AIL_set_3D_orientation@28", &p_AIL_set_3D_orientation},
    {"_AIL_set_3D_position@16", &p_AIL_set_3D_position},
    {"_AIL_set_3D_provider_preference@12", &p_AIL_set_3D_provider_preference},
    {"_AIL_set_3D_room_type@8", &p_AIL_set_3D_room_type},
    {"_AIL_set_3D_sample_cone@16", &p_AIL_set_3D_sample_cone},
    {"_AIL_set_3D_sample_distances@12", &p_AIL_set_3D_sample_distances},
    {"_AIL_set_3D_sample_effects_level@8", &p_AIL_set_3D_sample_effects_level},
    {"_AIL_set_3D_sample_file@8", &p_AIL_set_3D_sample_file},
    {"_AIL_set_3D_sample_info@8", &p_AIL_set_3D_sample_info},
    {"_AIL_set_3D_sample_loop_block@12", &p_AIL_set_3D_sample_loop_block},
    {"_AIL_set_3D_sample_loop_count@8", &p_AIL_set_3D_sample_loop_count},
    {"_AIL_set_3D_sample_obstruction@8", &p_AIL_set_3D_sample_obstruction},
    {"_AIL_set_3D_sample_occlusion@8", &p_AIL_set_3D_sample_occlusion},
    {"_AIL_set_3D_sample_offset@8", &p_AIL_set_3D_sample_offset},
    {"_AIL_set_3D_sample_playback_rate@8", &p_AIL_set_3D_sample_playback_rate},
    {"_AIL_set_3D_sample_preference@12", &p_AIL_set_3D_sample_preference},
    {"_AIL_set_3D_sample_volume@8", &p_AIL_set_3D_sample_volume},
    {"_AIL_set_3D_speaker_type@8", &p_AIL_set_3D_speaker_type},
    {"_AIL_set_3D_user_data@12", &p_AIL_set_3D_user_data},
    {"_AIL_set_3D_velocity@20", &p_AIL_set_3D_velocity},
    {"_AIL_set_3D_velocity_vector@16", &p_AIL_set_3D_velocity_vector},
    {"_AIL_set_DLS_processor@12", &p_AIL_set_DLS_processor},
    {"_AIL_set_DirectSound_HWND@8", &p_AIL_set_DirectSound_HWND},
    {"_AIL_set_XMIDI_master_volume@8", &p_AIL_set_XMIDI_master_volume},
    {"_AIL_set_digital_driver_processor@12", &p_AIL_set_digital_driver_processor},
    {"_AIL_set_digital_master_volume@8", &p_AIL_set_digital_master_volume},
    {"_AIL_set_error@4", &p_AIL_set_error},
    {"_AIL_set_file_async_callbacks@20", &p_AIL_set_file_async_callbacks},
    {"_AIL_set_file_callbacks@16", &p_AIL_set_file_callbacks},
    {"_AIL_set_filter_DLS_preference@12", &p_AIL_set_filter_DLS_preference},
    {"_AIL_set_filter_preference@12", &p_AIL_set_filter_preference},
    {"_AIL_set_filter_sample_preference@12", &p_AIL_set_filter_sample_preference},
    {"_AIL_set_filter_stream_preference@12", &p_AIL_set_filter_stream_preference},
    {"_AIL_set_input_state@8", &p_AIL_set_input_state},
    {"_AIL_set_named_sample_file@20", &p_AIL_set_named_sample_file},
    {"_AIL_set_preference@8", &p_AIL_set_preference},
    {"_AIL_set_redist_directory@4", &p_AIL_set_redist_directory},
    {"_AIL_set_sample_address@12", &p_AIL_set_sample_address},
    {"_AIL_set_sample_adpcm_block_size@8", &p_AIL_set_sample_adpcm_block_size},
    {"_AIL_set_sample_file@12", &p_AIL_set_sample_file},
    {"_AIL_set_sample_loop_block@12", &p_AIL_set_sample_loop_block},
    {"_AIL_set_sample_loop_count@8", &p_AIL_set_sample_loop_count},
    {"_AIL_set_sample_ms_position@8", &p_AIL_set_sample_ms_position},
    {"_AIL_set_sample_pan@8", &p_AIL_set_sample_pan},
    {"_AIL_set_sample_playback_rate@8", &p_AIL_set_sample_playback_rate},
    {"_AIL_set_sample_position@8", &p_AIL_set_sample_position},
    {"_AIL_set_sample_processor@12", &p_AIL_set_sample_processor},
    {"_AIL_set_sample_reverb@16", &p_AIL_set_sample_reverb},
    {"_AIL_set_sample_type@12", &p_AIL_set_sample_type},
    {"_AIL_set_sample_user_data@12", &p_AIL_set_sample_user_data},
    {"_AIL_set_sample_volume@8", &p_AIL_set_sample_volume},
    {"_AIL_set_sequence_loop_count@8", &p_AIL_set_sequence_loop_count},
    {"_AIL_set_sequence_ms_position@8", &p_AIL_set_sequence_ms_position},
    {"_AIL_set_sequence_tempo@12", &p_AIL_set_sequence_tempo},
    {"_AIL_set_sequence_user_data@12", &p_AIL_set_sequence_user_data},
    {"_AIL_set_sequence_volume@12", &p_AIL_set_sequence_volume},
    {"_AIL_set_stream_loop_block@12", &p_AIL_set_stream_loop_block},
    {"_AIL_set_stream_loop_count@8", &p_AIL_set_stream_loop_count},
    {"_AIL_set_stream_ms_position@8", &p_AIL_set_stream_ms_position},
    {"_AIL_set_stream_pan@8", &p_AIL_set_stream_pan},
    {"_AIL_set_stream_playback_rate@8", &p_AIL_set_stream_playback_rate},
    {"_AIL_set_stream_position@8", &p_AIL_set_stream_position},
    {"_AIL_set_stream_processor@12", &p_AIL_set_stream_processor},
    {"_AIL_set_stream_reverb@16", &p_AIL_set_stream_reverb},
    {"_AIL_set_stream_user_data@12", &p_AIL_set_stream_user_data},
    {"_AIL_set_stream_volume@8", &p_AIL_set_stream_volume},
    {"_AIL_set_timer_divisor@8", &p_AIL_set_timer_divisor},
    {"_AIL_set_timer_frequency@8", &p_AIL_set_timer_frequency},
    {"_AIL_set_timer_period@8", &p_AIL_set_timer_period},
    {"_AIL_set_timer_user@8", &p_AIL_set_timer_user},
    {"_AIL_shutdown@0", &p_AIL_shutdown},
    {"_AIL_size_processed_digital_audio@16", &p_AIL_size_processed_digital_audio},
    {"AIL_sprintf", &p_AIL_sprintf},
    {"_AIL_start_3D_sample@4", &p_AIL_start_3D_sample},
    {"_AIL_start_all_timers@0", &p_AIL_start_all_timers},
    {"_AIL_start_sample@4", &p_AIL_start_sample},
    {"_AIL_start_sequence@4", &p_AIL_start_sequence},
    {"_AIL_start_stream@4", &p_AIL_start_stream},
    {"_AIL_start_timer@4", &p_AIL_start_timer},
    {"_AIL_startup@0", &p_AIL_startup},
    {"_AIL_stop_3D_sample@4", &p_AIL_stop_3D_sample},
    {"_AIL_stop_all_timers@0", &p_AIL_stop_all_timers},
    {"_AIL_stop_sample@4", &p_AIL_stop_sample},
    {"_AIL_stop_sequence@4", &p_AIL_stop_sequence},
    {"_AIL_stop_timer@4", &p_AIL_stop_timer},
    {"_AIL_stream_info@20", &p_AIL_stream_info},
    {"_AIL_stream_loop_count@4", &p_AIL_stream_loop_count},
    {"_AIL_stream_ms_position@12", &p_AIL_stream_ms_position},
    {"_AIL_stream_pan@4", &p_AIL_stream_pan},
    {"_AIL_stream_playback_rate@4", &p_AIL_stream_playback_rate},
    {"_AIL_stream_position@4", &p_AIL_stream_position},
    {"_AIL_stream_reverb@16", &p_AIL_stream_reverb},
    {"_AIL_stream_status@4", &p_AIL_stream_status},
    {"_AIL_stream_user_data@8", &p_AIL_stream_user_data},
    {"_AIL_stream_volume@4", &p_AIL_stream_volume},
    {"_AIL_true_sequence_channel@8", &p_AIL_true_sequence_channel},
    {"_AIL_unlock@0", &p_AIL_unlock},
    {"_AIL_unlock_mutex@0", &p_AIL_unlock_mutex},
    {"_AIL_update_3D_position@8", &p_AIL_update_3D_position},
    {"_AIL_us_count@0", &p_AIL_us_count},
    {"_AIL_waveOutClose@4", &p_AIL_waveOutClose},
    {"_AIL_waveOutOpen@16", &p_AIL_waveOutOpen},
    {"DLSClose", &p_DLSClose},
    {"DLSCompactMemory", &p_DLSCompactMemory},
    {"DLSGetInfo", &p_DLSGetInfo},
    {"DLSLoadFile", &p_DLSLoadFile},
    {"DLSLoadMemFile", &p_DLSLoadMemFile},
    {"_DLSMSSGetCPU@4", &p_DLSMSSGetCPU},
    {"DLSMSSOpen", &p_DLSMSSOpen},
    {"DLSSetAttribute", &p_DLSSetAttribute},
    {"DLSUnloadAll", &p_DLSUnloadAll},
    {"DLSUnloadFile", &p_DLSUnloadFile},
    {"_DllMain@12", &p_DllMain},
    {"RIB_alloc_provider_handle", &p_RIB_alloc_provider_handle},
    {"RIB_enumerate_interface", &p_RIB_enumerate_interface},
    {"_RIB_enumerate_providers@12", &p_RIB_enumerate_providers},
    {"RIB_error", &p_RIB_error},
    {"_RIB_find_file_dec_provider@20", &p_RIB_find_file_dec_provider},
    {"RIB_find_file_provider", &p_RIB_find_file_provider},
    {"_RIB_find_files_provider@20", &p_RIB_find_files_provider},
    {"_RIB_find_provider@12", &p_RIB_find_provider},
    {"RIB_free_provider_handle", &p_RIB_free_provider_handle},
    {"RIB_free_provider_library", &p_RIB_free_provider_library},
    {"_RIB_load_application_providers@4", &p_RIB_load_application_providers},
    {"RIB_load_provider_library", &p_RIB_load_provider_library},
    {"_RIB_provider_library_handle@0", &p_RIB_provider_library_handle},
    {"_RIB_provider_system_data@8", &p_RIB_provider_system_data},
    {"_RIB_provider_user_data@8", &p_RIB_provider_user_data},
    {"RIB_register_interface", &p_RIB_register_interface},
    {"RIB_request_interface", &p_RIB_request_interface},
    {"RIB_request_interface_entry", &p_RIB_request_interface_entry},
    {"_RIB_set_provider_system_data@12", &p_RIB_set_provider_system_data},
    {"_RIB_set_provider_user_data@12", &p_RIB_set_provider_user_data},
    {"RIB_type_string", &p_RIB_type_string},
    {"RIB_unregister_interface", &p_RIB_unregister_interface},
};
/* GENERATED-END */

/* ------------------------------------------------------------------ the log */

static CRITICAL_SECTION Lock;
static HANDLE LogFile = INVALID_HANDLE_VALUE;
static int Enabled = -1;            /* -1 not decided yet, 0 off, 1 on */
static unsigned long Index = 0;
static LARGE_INTEGER Freq, T0;
static char *ImageLo = 0, *ImageHi = 0;
static HMODULE Real = 0;
static char Line[4096];
static int Len = 0;
typedef unsigned long (__stdcall *FrameFn)(void);
static FrameFn Frame = 0;
static int FrameState = 0;          /* 0 look again, 1 found, 2 d3d8.dll has no such export */

static void Put(const char *s)
{
    int n = (int)strlen(s);
    if (Len + n >= (int)sizeof(Line) - 4)
        n = (int)sizeof(Line) - 4 - Len;
    if (n > 0) {
        memcpy(Line + Len, s, n);
        Len += n;
    }
}

static void Put_Str(const char *s)
{
    char b[8];
    Put("\"");
    if (s) {
        for (; *s; s++) {
            unsigned char c = (unsigned char)*s;
            if (c == '"' || c == '\\') {
                b[0] = '\\'; b[1] = c; b[2] = 0; Put(b);
            } else if (c < 0x20 || c >= 0x7F) {
                sprintf(b, "\\u%04x", c); Put(b);
            } else {
                b[0] = c; b[1] = 0; Put(b);
            }
        }
    }
    Put("\"");
}

static void Kv_S(const char *k, const char *v)
{
    Put(",\""); Put(k); Put("\":"); Put_Str(v);
}

static void Kv_I(const char *k, long v)
{
    char b[32];
    sprintf(b, ",\"%s\":%ld", k, v);
    Put(b);
}

static void Kv_F(const char *k, float v)
{
    char b[64];
    if (!_finite(v))
        sprintf(b, ",\"%s\":\"%s\"", k, _isnan(v) ? "nan" : (v < 0 ? "-inf" : "inf"));
    else
        sprintf(b, ",\"%s\":%.9g", k, (double)v);
    Put(b);
}

static void Header(void);
static void Line_Start(const char *ev, unsigned long index);

static void Open_Log(void)
{
    char path[MAX_PATH];
    DWORD n = GetEnvironmentVariableA("MSS32LOG_FILE", path, sizeof(path));
    Enabled = 0;
    if (n == 0 || n >= sizeof(path))
        return;
    LogFile = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL, 0);
    if (LogFile == INVALID_HANDLE_VALUE)
        return;
    QueryPerformanceFrequency(&Freq);
    QueryPerformanceCounter(&T0);
    Enabled = 1;
    Header();
}

static void Frame_Look(void)
{
    HMODULE d3d;
    if (FrameState)
        return;
    d3d = GetModuleHandleA("d3d8.dll");
    if (!d3d)
        return;
    Frame = (FrameFn)GetProcAddress(d3d, "D3d8shot_Frame");
    FrameState = Frame ? 1 : 2;
}

/* Whether a call from caller is logged: logging not off, and the caller in the exe's image. */
static int Want(void *caller)
{
    if (Enabled == 0)
        return 0;
    if (caller && ImageLo && ((char *)caller < ImageLo || (char *)caller >= ImageHi))
        return 0;
    return 1;
}

static void Line_Start(const char *ev, unsigned long index)
{
    char b[96];
    LARGE_INTEGER now;
    Frame_Look();
    QueryPerformanceCounter(&now);
    Len = 0;
    sprintf(b, "{\"i\":%lu,\"t\":%lu,\"tick\":%lu", index,
            (unsigned long)((now.QuadPart - T0.QuadPart) * 1000 / Freq.QuadPart), GetTickCount());
    Put(b);
    if (Frame) {
        sprintf(b, ",\"f\":%lu", Frame());
        Put(b);
    }
    Kv_S("ev", ev);
}

/* Starts an event line (and takes the lock); returns 0 when logging is off or the caller is not the exe. */
static int Ev(const char *ev, void *caller)
{
    if (!Want(caller))
        return 0;
    EnterCriticalSection(&Lock);
    if (Enabled < 0)
        Open_Log();
    if (!Enabled) {
        LeaveCriticalSection(&Lock);
        return 0;
    }
    Line_Start(ev, ++Index);
    return 1;
}

static void Ev_End(void)
{
    DWORD w;
    Put("}\n");
    WriteFile(LogFile, Line, Len, &w, 0);
    LeaveCriticalSection(&Lock);
}

/* The return address of the wrapper that expands it (a function with inline asm always has an EBP frame). */
#define CALLER(v) __asm { mov eax, [ebp + 4] } __asm { mov v, eax }

/* ------------------------------------------------------------------ handles -> ids */

typedef struct {
    unsigned long key;
    char kind;
    int id;
    int live;
    char snd[80];       /* the handle's sound: "hash:size" for memory samples, the file for streams */
} Handle;

static Handle Handles[4096];
static int Handle_Count = 0;
static int Next_Id[128];

static Handle *Find(unsigned long key)
{
    int k;
    for (k = Handle_Count - 1; k >= 0; k--)
        if (Handles[k].live && Handles[k].key == key)
            return &Handles[k];
    return 0;
}

static Handle *New_Handle(unsigned long key, char kind)
{
    Handle *h;
    int k;
    for (k = 0; k < Handle_Count; k++)          /* a released slot is reused; ids never are */
        if (!Handles[k].live)
            break;
    if (k == Handle_Count) {
        if (Handle_Count == sizeof(Handles) / sizeof(Handles[0]))
            k = 0;
        else
            Handle_Count++;
    }
    h = &Handles[k];
    h->key = key;
    h->kind = kind;
    h->id = ++Next_Id[(unsigned char)kind & 127];
    h->live = 1;
    h->snd[0] = 0;
    return h;
}

static Handle *Get(unsigned long key, char kind)
{
    Handle *h = Find(key);
    return h ? h : New_Handle(key, kind);
}

/* Logs the handle under the key "h" (or k): its id, "-" for null. Unknown handles get a u id. */
static Handle *Kv_H(const char *k, unsigned long key)
{
    char b[24];
    Handle *h;
    if (!key) {
        Kv_S(k, "-");
        return 0;
    }
    h = Get(key, 'u');
    sprintf(b, "%c%d", h->kind, h->id);
    Kv_S(k, b);
    return h;
}

static Handle *Kv_New(const char *k, unsigned long key, char kind)
{
    Handle *h = Find(key);
    if (h)
        h->live = 0;            /* the same pointer back from an allocation: a new handle */
    if (!key) {
        Kv_S(k, "-");
        return 0;
    }
    h = New_Handle(key, kind);
    return Kv_H(k, key);
}

static void Kv_Snd(Handle *h)
{
    if (h && h->snd[0])
        Kv_S("snd", h->snd);
}

static void Release(unsigned long key)
{
    Handle *h = Find(key);
    if (h)
        h->live = 0;
}

/* FNV-1a 64 over the bytes, as 16 hex digits; the sample's sound becomes "hash:size". */
static void Kv_Content(Handle *h, const void *data, long size)
{
    unsigned __int64 x = 0xcbf29ce484222325ui64;
    const unsigned char *p = (const unsigned char *)data;
    char b[40];
    long k;
    if (!data || size <= 0) {
        Kv_I("size", size);
        if (h)
            strcpy(h->snd, data ? "empty" : "null");
        return;
    }
    __try {
        for (k = 0; k < size; k++) {
            x ^= p[k];
            x *= 0x100000001b3ui64;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        size = -k - 1;      /* unreadable after k bytes: logged as a negative size */
    }
    sprintf(b, "%08lx%08lx", (unsigned long)(x >> 32), (unsigned long)x);
    Kv_I("size", size);
    Kv_S("hash", b);
    if (h)
        sprintf(h->snd, "%s:%ld", b, size);
}

/* A memory image's length when the call gives none: a RIFF file's own, else 0 (not hashed). */
static long Riff_Size(const void *data)
{
    const unsigned char *p = (const unsigned char *)data;
    long n = 0;
    if (!p)
        return 0;
    __try {
        if (p[0] == 'R' && p[1] == 'I' && p[2] == 'F' && p[3] == 'F')
            n = (long)(*(const unsigned long *)(p + 4)) + 8;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        n = 0;
    }
    return n;
}

/* ------------------------------------------------------------------ the logged calls (mss.h 6.1a) */

#define R(n) p_##n
typedef void *H;

typedef long (__stdcall *T_l_v)(void);
typedef void (__stdcall *T_v_v)(void);
typedef void (__stdcall *T_v_h)(H);
typedef long (__stdcall *T_l_h)(H);
typedef void (__stdcall *T_v_hl)(H, long);
typedef void (__stdcall *T_v_hll)(H, long, long);
typedef long (__stdcall *T_l_ll)(long, long);
typedef void (__stdcall *T_v_hpp)(H, long *, long *);

/* The sound's position: kind 2 a sample (ms), 1 a stream (ms), 3 a 3D sample (bytes). Read before Ev takes
   the lock, only for a call that is logged. */
typedef struct { int kind; long cur, total; } Pos;

static void Pos_Read(Pos *p, int kind, H s, void *c)
{
    p->kind = 0;
    p->cur = p->total = -1;
    if (!s || !Want(c))
        return;
    p->kind = kind;
    if (kind == 1)
        ((T_v_hpp)R(AIL_stream_ms_position))(s, &p->total, &p->cur);
    else if (kind == 2)
        ((T_v_hpp)R(AIL_sample_ms_position))(s, &p->total, &p->cur);
    else {
        p->cur = (long)((T_l_h)R(AIL_3D_sample_offset))(s);
        p->total = (long)((T_l_h)R(AIL_3D_sample_length))(s);
    }
}

static void Kv_Pos(const Pos *p)
{
    if (!p->kind)
        return;
    Kv_I(p->kind == 3 ? "pos" : "pos_ms", p->cur);
    Kv_I(p->kind == 3 ? "len" : "len_ms", p->total);
}

D long __stdcall AIL_startup(void)
{
    void *c; long r;
    CALLER(c);
    r = ((T_l_v)R(AIL_startup))();
    if (Ev("startup", c)) { Kv_I("ret", r); Ev_End(); }
    return r;
}

D void __stdcall AIL_shutdown(void)
{
    void *c;
    CALLER(c);
    if (Ev("shutdown", c)) Ev_End();
    ((T_v_v)R(AIL_shutdown))();
}

D long __stdcall AIL_set_preference(unsigned long number, long value)
{
    void *c; long r;
    CALLER(c);
    r = ((T_l_ll)R(AIL_set_preference))((long)number, value);
    if (Ev("preference", c)) { Kv_I("n", (long)number); Kv_I("v", value); Ev_End(); }
    return r;
}

typedef long (__stdcall *T_waveOutOpen)(H *, void *, long, void *);
D long __stdcall AIL_waveOutOpen(H *drvr, void *lphWaveOut, long wDeviceID, void *lpFormat)
{
    void *c; long r;
    CALLER(c);
    r = ((T_waveOutOpen)R(AIL_waveOutOpen))(drvr, lphWaveOut, wDeviceID, lpFormat);
    if (Ev("driver_open", c)) {
        Kv_S("api", "waveOut");
        Kv_New("h", drvr ? (unsigned long)*drvr : 0, 'd');
        Kv_I("device", wDeviceID);
        if (lpFormat) {         /* WAVEFORMAT: tag, channels, rate, bytes/s, align (PCMWAVEFORMAT adds bits) */
            WAVEFORMAT *f = (WAVEFORMAT *)lpFormat;
            Kv_I("rate", (long)f->nSamplesPerSec);
            Kv_I("ch", (long)f->nChannels);
            if (f->wFormatTag == WAVE_FORMAT_PCM)
                Kv_I("bits", (long)((PCMWAVEFORMAT *)lpFormat)->wBitsPerSample);
        }
        Kv_I("ret", r);
        Ev_End();
    }
    return r;
}

D void __stdcall AIL_waveOutClose(H drvr)
{
    void *c;
    CALLER(c);
    if (Ev("driver_close", c)) { Kv_H("h", (unsigned long)drvr); Ev_End(); }
    Release((unsigned long)drvr);
    ((T_v_h)R(AIL_waveOutClose))(drvr);
}

typedef H (__stdcall *T_open_digital)(unsigned long, long, long, unsigned long);
D H __stdcall AIL_open_digital_driver(unsigned long frequency, long bits, long channel, unsigned long flags)
{
    void *c; H r;
    CALLER(c);
    r = ((T_open_digital)R(AIL_open_digital_driver))(frequency, bits, channel, flags);
    if (Ev("driver_open", c)) {
        Kv_S("api", "digital"); Kv_New("h", (unsigned long)r, 'd');
        Kv_I("rate", (long)frequency); Kv_I("ch", channel); Kv_I("bits", bits); Kv_I("flags", (long)flags);
        Ev_End();
    }
    return r;
}

D void __stdcall AIL_close_digital_driver(H dig)
{
    void *c;
    CALLER(c);
    if (Ev("driver_close", c)) { Kv_H("h", (unsigned long)dig); Ev_End(); }
    Release((unsigned long)dig);
    ((T_v_h)R(AIL_close_digital_driver))(dig);
}

D void __stdcall AIL_set_digital_master_volume(H dig, long v)
{
    void *c;
    CALLER(c);
    ((T_v_hl)R(AIL_set_digital_master_volume))(dig, v);
    if (Ev("master_volume", c)) { Kv_H("h", (unsigned long)dig); Kv_I("v", v); Ev_End(); }
}

/* ---- 2D samples */

D H __stdcall AIL_allocate_sample_handle(H dig)
{
    void *c; H r;
    CALLER(c);
    r = (H)((T_l_h)R(AIL_allocate_sample_handle))(dig);
    if (Ev("alloc", c)) { Kv_New("h", (unsigned long)r, 's'); Kv_H("driver", (unsigned long)dig); Ev_End(); }
    return r;
}

D void __stdcall AIL_release_sample_handle(H s)
{
    void *c;
    CALLER(c);
    if (Ev("release", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Ev_End(); }
    Release((unsigned long)s);
    ((T_v_h)R(AIL_release_sample_handle))(s);
}

D void __stdcall AIL_init_sample(H s)
{
    void *c;
    CALLER(c);
    ((T_v_h)R(AIL_init_sample))(s);
    if (Ev("init", c)) { Handle *h = Kv_H("h", (unsigned long)s); if (h) h->snd[0] = 0; Ev_End(); }
}

typedef long (__stdcall *T_set_sample_file)(H, const void *, long);
D long __stdcall AIL_set_sample_file(H s, const void *image, long block)
{
    void *c; long r;
    CALLER(c);
    r = ((T_set_sample_file)R(AIL_set_sample_file))(s, image, block);
    if (Ev("load", c)) {
        Handle *h = Kv_H("h", (unsigned long)s);
        Kv_Content(h, image, Riff_Size(image)); Kv_I("block", block); Kv_I("ret", r); Ev_End();
    }
    return r;
}

typedef long (__stdcall *T_set_named)(H, const char *, const void *, long, long);
D long __stdcall AIL_set_named_sample_file(H s, const char *suffix, const void *image, long size, long block)
{
    void *c; long r;
    CALLER(c);
    r = ((T_set_named)R(AIL_set_named_sample_file))(s, suffix, image, size, block);
    if (Ev("load", c)) {
        Handle *h = Kv_H("h", (unsigned long)s);
        Kv_S("type", suffix);
        Kv_Content(h, image, size > 0 ? size : Riff_Size(image));
        Kv_I("block", block); Kv_I("ret", r); Ev_End();
    }
    return r;
}

typedef unsigned long (__stdcall *T_set_proc)(H, long, unsigned long);
D unsigned long __stdcall AIL_set_sample_processor(H s, long stage, unsigned long provider)
{
    void *c; unsigned long r;
    CALLER(c);
    r = ((T_set_proc)R(AIL_set_sample_processor))(s, stage, provider);
    if (Ev("processor", c)) { Kv_H("h", (unsigned long)s); Kv_I("stage", stage); Kv_H("provider", provider); Ev_End(); }
    return r;
}

static void Simple(const char *ev, void *c, H s)
{
    if (Ev(ev, c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Ev_End(); }
}

static void Value(const char *ev, void *c, H s, long v)
{
    if (Ev(ev, c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_I("v", v); Ev_End(); }
}

D void __stdcall AIL_start_sample(H s)
{
    void *c;
    CALLER(c);
    ((T_v_h)R(AIL_start_sample))(s);
    Simple("start", c, s);
}

D void __stdcall AIL_stop_sample(H s)
{
    void *c; Pos p;
    CALLER(c);
    Pos_Read(&p, 2, s, c);
    if (Ev("stop", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_Pos(&p); Ev_End(); }
    ((T_v_h)R(AIL_stop_sample))(s);
}

D void __stdcall AIL_resume_sample(H s)
{
    void *c;
    CALLER(c);
    ((T_v_h)R(AIL_resume_sample))(s);
    Simple("resume", c, s);
}

D void __stdcall AIL_end_sample(H s)
{
    void *c; Pos p;
    CALLER(c);
    Pos_Read(&p, 2, s, c);
    if (Ev("end", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_Pos(&p); Ev_End(); }
    ((T_v_h)R(AIL_end_sample))(s);
}

#define SETTER(fn, ev) \
    D void __stdcall fn(H s, long v) \
    { \
        void *c; \
        CALLER(c); \
        ((T_v_hl)R(fn))(s, v); \
        Value(ev, c, s, v); \
    }

SETTER(AIL_set_sample_volume, "volume")
SETTER(AIL_set_sample_pan, "pan")
SETTER(AIL_set_sample_playback_rate, "rate")
SETTER(AIL_set_sample_loop_count, "loop")
SETTER(AIL_set_sample_ms_position, "seek_ms")
SETTER(AIL_set_sample_position, "seek")

/* ---- streams */

typedef H (__stdcall *T_open_stream)(H, const char *, long);
D H __stdcall AIL_open_stream(H dig, const char *file, long mem)
{
    void *c; H r;
    CALLER(c);
    r = ((T_open_stream)R(AIL_open_stream))(dig, file, mem);
    if (Ev("open", c)) {
        Handle *h = Kv_New("h", (unsigned long)r, 'm');
        Kv_H("driver", (unsigned long)dig); Kv_S("file", file); Kv_I("mem", mem);
        if (h && file) { strncpy(h->snd, file, sizeof(h->snd) - 1); h->snd[sizeof(h->snd) - 1] = 0; }
        Ev_End();
    }
    return r;
}

typedef H (__stdcall *T_open_stream_s)(H, H, const char *, long);
D H __stdcall AIL_open_stream_by_sample(H dig, H s, const char *file, long mem)
{
    void *c; H r;
    CALLER(c);
    r = ((T_open_stream_s)R(AIL_open_stream_by_sample))(dig, s, file, mem);
    if (Ev("open", c)) {
        Handle *h = Kv_New("h", (unsigned long)r, 'm');
        Kv_H("driver", (unsigned long)dig); Kv_H("sample", (unsigned long)s); Kv_S("file", file); Kv_I("mem", mem);
        if (h && file) { strncpy(h->snd, file, sizeof(h->snd) - 1); h->snd[sizeof(h->snd) - 1] = 0; }
        Ev_End();
    }
    return r;
}

D void __stdcall AIL_close_stream(H m)
{
    void *c; Pos p;
    CALLER(c);
    Pos_Read(&p, 1, m, c);
    if (Ev("close", c)) { Kv_Snd(Kv_H("h", (unsigned long)m)); Kv_Pos(&p); Ev_End(); }
    Release((unsigned long)m);
    ((T_v_h)R(AIL_close_stream))(m);
}

D void __stdcall AIL_start_stream(H m)
{
    void *c;
    CALLER(c);
    ((T_v_h)R(AIL_start_stream))(m);
    Simple("start", c, m);
}

D void __stdcall AIL_pause_stream(H m, long on)
{
    void *c; Pos p;
    CALLER(c);
    Pos_Read(&p, 1, m, c);
    if (Ev(on ? "pause" : "resume", c)) { Kv_Snd(Kv_H("h", (unsigned long)m)); Kv_Pos(&p); Ev_End(); }
    ((T_v_hl)R(AIL_pause_stream))(m, on);
}

SETTER(AIL_set_stream_volume, "volume")
SETTER(AIL_set_stream_pan, "pan")
SETTER(AIL_set_stream_playback_rate, "rate")
SETTER(AIL_set_stream_loop_count, "loop")
SETTER(AIL_set_stream_ms_position, "seek_ms")
SETTER(AIL_set_stream_position, "seek")

D void __stdcall AIL_set_stream_loop_block(H m, long start, long end)
{
    void *c;
    CALLER(c);
    ((T_v_hll)R(AIL_set_stream_loop_block))(m, start, end);
    if (Ev("loop_block", c)) { Kv_Snd(Kv_H("h", (unsigned long)m)); Kv_I("start", start); Kv_I("end", end); Ev_End(); }
}

/* ---- providers, filters, the listener */

typedef long (__stdcall *T_enum)(long *, unsigned long *, char **);
static long Enumerate(FARPROC fn, const char *ev, char kind, void *c, long *next, unsigned long *dest, char **name)
{
    long r = ((T_enum)fn)(next, dest, name);
    if (r && dest && Ev(ev, c)) {
        Handle *h = Find(*dest);
        if (!h)
            Kv_New("h", *dest, kind);
        else
            Kv_H("h", *dest);
        Kv_S("name", name ? *name : 0);
        Ev_End();
    }
    return r;
}

D long __stdcall AIL_enumerate_3D_providers(long *next, unsigned long *dest, char **name)
{
    void *c;
    CALLER(c);
    return Enumerate(R(AIL_enumerate_3D_providers), "provider_enum", 'p', c, next, dest, name);
}

D long __stdcall AIL_enumerate_filters(long *next, unsigned long *dest, char **name)
{
    void *c;
    CALLER(c);
    return Enumerate(R(AIL_enumerate_filters), "filter_enum", 'f', c, next, dest, name);
}

D long __stdcall AIL_open_3D_provider(unsigned long lib)
{
    void *c; long r;
    CALLER(c);
    r = ((T_l_h)R(AIL_open_3D_provider))((H)lib);
    if (Ev("provider_open", c)) { Kv_H("h", lib); Kv_I("ret", r); Ev_End(); }
    return r;
}

D void __stdcall AIL_close_3D_provider(unsigned long lib)
{
    void *c;
    CALLER(c);
    if (Ev("provider_close", c)) { Kv_H("h", lib); Ev_End(); }
    ((T_v_h)R(AIL_close_3D_provider))((H)lib);
}

D H __stdcall AIL_open_3D_listener(unsigned long lib)
{
    void *c; H r;
    CALLER(c);
    r = (H)((T_l_h)R(AIL_open_3D_listener))((H)lib);
    if (Ev("listener_open", c)) { Kv_New("h", (unsigned long)r, 'l'); Kv_H("provider", lib); Ev_End(); }
    return r;
}

D void __stdcall AIL_close_3D_listener(H l)
{
    void *c;
    CALLER(c);
    if (Ev("listener_close", c)) { Kv_H("h", (unsigned long)l); Ev_End(); }
    Release((unsigned long)l);
    ((T_v_h)R(AIL_close_3D_listener))(l);
}

D void __stdcall AIL_set_3D_speaker_type(unsigned long lib, long v)
{
    void *c;
    CALLER(c);
    ((T_v_hl)R(AIL_set_3D_speaker_type))((H)lib, v);
    if (Ev("speaker_type", c)) { Kv_H("provider", lib); Kv_I("v", v); Ev_End(); }
}

D void __stdcall AIL_set_3D_room_type(unsigned long lib, long v)
{
    void *c;
    CALLER(c);
    ((T_v_hl)R(AIL_set_3D_room_type))((H)lib, v);
    if (Ev("room_type", c)) { Kv_H("provider", lib); Kv_I("v", v); Ev_End(); }
}

typedef void (__stdcall *T_filter_pref)(H, const char *, const void *);
D void __stdcall AIL_set_filter_sample_preference(H s, const char *name, const void *val)
{
    void *c;
    CALLER(c);
    ((T_filter_pref)R(AIL_set_filter_sample_preference))(s, name, val);
    if (Ev("filter_pref", c)) {
        Kv_H("h", (unsigned long)s); Kv_S("name", name);
        if (val) {
            __try { Kv_I("v", *(const long *)val); } __except (EXCEPTION_EXECUTE_HANDLER) { Kv_S("v", "?"); }
        }
        Ev_End();
    }
}

/* ---- 3D samples */

D H __stdcall AIL_allocate_3D_sample_handle(unsigned long lib)
{
    void *c; H r;
    CALLER(c);
    r = (H)((T_l_h)R(AIL_allocate_3D_sample_handle))((H)lib);
    if (Ev("alloc", c)) { Kv_New("h", (unsigned long)r, 't'); Kv_H("provider", lib); Ev_End(); }
    return r;
}

D void __stdcall AIL_release_3D_sample_handle(H s)
{
    void *c;
    CALLER(c);
    if (Ev("release", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Ev_End(); }
    Release((unsigned long)s);
    ((T_v_h)R(AIL_release_3D_sample_handle))(s);
}

typedef long (__stdcall *T_set_3D_file)(H, const void *);
D long __stdcall AIL_set_3D_sample_file(H s, const void *image)
{
    void *c; long r;
    CALLER(c);
    r = ((T_set_3D_file)R(AIL_set_3D_sample_file))(s, image);
    if (Ev("load", c)) {
        Handle *h = Kv_H("h", (unsigned long)s);
        Kv_Content(h, image, Riff_Size(image)); Kv_I("ret", r); Ev_End();
    }
    return r;
}

D void __stdcall AIL_start_3D_sample(H s)
{
    void *c;
    CALLER(c);
    ((T_v_h)R(AIL_start_3D_sample))(s);
    Simple("start", c, s);
}

D void __stdcall AIL_stop_3D_sample(H s)
{
    void *c; Pos p;
    CALLER(c);
    Pos_Read(&p, 3, s, c);
    if (Ev("stop", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_Pos(&p); Ev_End(); }
    ((T_v_h)R(AIL_stop_3D_sample))(s);
}

D void __stdcall AIL_resume_3D_sample(H s)
{
    void *c;
    CALLER(c);
    ((T_v_h)R(AIL_resume_3D_sample))(s);
    Simple("resume", c, s);
}

D void __stdcall AIL_end_3D_sample(H s)
{
    void *c; Pos p;
    CALLER(c);
    Pos_Read(&p, 3, s, c);
    if (Ev("end", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_Pos(&p); Ev_End(); }
    ((T_v_h)R(AIL_end_3D_sample))(s);
}

SETTER(AIL_set_3D_sample_volume, "volume")
SETTER(AIL_set_3D_sample_playback_rate, "rate")
SETTER(AIL_set_3D_sample_offset, "seek")
SETTER(AIL_set_3D_sample_loop_count, "loop")

typedef void (__stdcall *T_v_hff)(H, float, float);
D void __stdcall AIL_set_3D_sample_distances(H s, float max_dist, float min_dist)
{
    void *c;
    CALLER(c);
    ((T_v_hff)R(AIL_set_3D_sample_distances))(s, max_dist, min_dist);
    if (Ev("distances", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_F("max", max_dist); Kv_F("min", min_dist); Ev_End(); }
}

typedef void (__stdcall *T_v_hf)(H, float);
D void __stdcall AIL_set_3D_sample_effects_level(H s, float level)
{
    void *c;
    CALLER(c);
    ((T_v_hf)R(AIL_set_3D_sample_effects_level))(s, level);
    if (Ev("effects", c)) { Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_F("v", level); Ev_End(); }
}

typedef void (__stdcall *T_cone)(H, float, float, long);
D void __stdcall AIL_set_3D_sample_cone(H s, float inner, float outer, long outer_volume)
{
    void *c;
    CALLER(c);
    ((T_cone)R(AIL_set_3D_sample_cone))(s, inner, outer, outer_volume);
    if (Ev("cone", c)) {
        Kv_Snd(Kv_H("h", (unsigned long)s)); Kv_F("inner", inner); Kv_F("outer", outer); Kv_I("outer_v", outer_volume);
        Ev_End();
    }
}

/* ---- 3D objects: a 3D sample or the listener */

typedef void (__stdcall *T_v_hfff)(H, float, float, float);
D void __stdcall AIL_set_3D_position(H o, float x, float y, float z)
{
    void *c;
    CALLER(c);
    ((T_v_hfff)R(AIL_set_3D_position))(o, x, y, z);
    if (Ev("position", c)) { Kv_Snd(Kv_H("h", (unsigned long)o)); Kv_F("x", x); Kv_F("y", y); Kv_F("z", z); Ev_End(); }
}

typedef void (__stdcall *T_v_hffff)(H, float, float, float, float);
D void __stdcall AIL_set_3D_velocity(H o, float dx, float dy, float dz, float mag)
{
    void *c;
    CALLER(c);
    ((T_v_hffff)R(AIL_set_3D_velocity))(o, dx, dy, dz, mag);
    if (Ev("velocity", c)) {
        Kv_Snd(Kv_H("h", (unsigned long)o)); Kv_F("dx", dx); Kv_F("dy", dy); Kv_F("dz", dz); Kv_F("mag", mag);
        Ev_End();
    }
}

D void __stdcall AIL_set_3D_velocity_vector(H o, float dx, float dy, float dz)
{
    void *c;
    CALLER(c);
    ((T_v_hfff)R(AIL_set_3D_velocity_vector))(o, dx, dy, dz);
    if (Ev("velocity", c)) { Kv_Snd(Kv_H("h", (unsigned long)o)); Kv_F("dx", dx); Kv_F("dy", dy); Kv_F("dz", dz); Ev_End(); }
}

typedef void (__stdcall *T_orient)(H, float, float, float, float, float, float);
D void __stdcall AIL_set_3D_orientation(H o, float fx, float fy, float fz, float ux, float uy, float uz)
{
    void *c;
    CALLER(c);
    ((T_orient)R(AIL_set_3D_orientation))(o, fx, fy, fz, ux, uy, uz);
    if (Ev("orientation", c)) {
        Kv_Snd(Kv_H("h", (unsigned long)o));
        Kv_F("fx", fx); Kv_F("fy", fy); Kv_F("fz", fz); Kv_F("ux", ux); Kv_F("uy", uy); Kv_F("uz", uz);
        Ev_End();
    }
}

/* ------------------------------------------------------------------ loading the real DLL */

static int Missing = 0;

static void Header(void)
{
    char path[MAX_PATH];
    DWORD w;
    Line_Start("log", 0);
    Kv_I("version", 1);
    GetModuleFileNameA(Real, path, sizeof(path));
    Kv_S("real", strrchr(path, '\\') ? strrchr(path, '\\') + 1 : path);
    GetModuleFileNameA(0, path, sizeof(path));
    Kv_S("exe", strrchr(path, '\\') ? strrchr(path, '\\') + 1 : path);
    Kv_I("frames", Frame ? 1 : 0);
    Kv_I("missing", Missing);
    Put("}\n");
    WriteFile(LogFile, Line, Len, &w, 0);
}

/* Mss32.dll exports its DllMain (_DllMain@12): so does the proxy, its own. */
D BOOL __stdcall DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
    char path[MAX_PATH], *slash;
    int k;
    HMODULE exe;
    (void)reserved;
    if (reason != DLL_PROCESS_ATTACH)
        return TRUE;
    DisableThreadLibraryCalls(inst);
    InitializeCriticalSection(&Lock);
    GetModuleFileNameA(inst, path, sizeof(path) - 16);
    slash = strrchr(path, '\\');
    strcpy(slash ? slash + 1 : path, "mss32r.dll");
    Real = LoadLibraryA(path);
    if (!Real)
        return FALSE;
    for (k = 0; k < (int)(sizeof(Table) / sizeof(Table[0])); k++) {
        *Table[k].p = GetProcAddress(Real, Table[k].name);
        if (!*Table[k].p)
            Missing++;          /* logged in the header; a missing export is never called by this DLL */
    }
    exe = GetModuleHandleA(0);
    if (exe) {
        IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)exe;
        IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)((char *)exe + dos->e_lfanew);
        ImageLo = (char *)exe;
        ImageHi = ImageLo + nt->OptionalHeader.SizeOfImage;
    }
    return TRUE;
}

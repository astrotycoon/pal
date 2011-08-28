#include "libpal/libpal.h"

bool PalProcessTest() {
  int r;
  palProcessStartDescription spawned_desc;
  palPipe p_stdin;
  palPipe p_stderr;
  palPipe p_stdout;
  r = p_stdin.Create(true);
  if (r) {
    palBreakHere();
  }
  r = p_stderr.Create(true);
  if (r) {
    palBreakHere();
  }
  r = p_stdout.Create(true);
  if (r) {
    palBreakHere();
  }

  palPipeStream ps_stdin;
  palPipeStream ps_stdout;

  r = ps_stdin.CreateWriteStream(&p_stdin);
  if (r) {
    palBreakHere();
  }
  r = ps_stdout.CreateReadStream(&p_stdout);
  if (r) {
    palBreakHere();
  }

  unsigned char buff[4096];
  buff[4095] = '\0';
  const char* msg = "It_came_from_somewhere_else.\n";
  {
    spawned_desc.command_line = palStringDuplicate("C:\\workspace\\pal\\Release\\pal_process_start_test.exe testing 1 2 3 4 5");
    spawned_desc.stdin_override = &p_stdin;
    spawned_desc.stderr_override = &p_stderr;
    spawned_desc.stdout_override = &p_stdout;
    spawned_desc.inherit_os_objects = true;
    palProcess spawned_process;
    r = palProcess::Start(spawned_desc, &spawned_process);
    if (r != 0) {
      palPrintf("Could not start process: %s\n", spawned_desc.command_line);
      return false;
    }
    {
      // send a string to the spawned process
      uint64_t ln;
      r = ps_stdin.Write(msg, 0, strlen(msg), &ln);
      if (r) {
        palBreakHere();
      }
      ps_stdin.Flush();
      p_stdin.CloseWrite();
    }
    palStringDuplicateDeallocate(spawned_desc.command_line);
    while (true) {
      bool exited = spawned_process.HasExited();
      if (exited) {
        break;
      }
    }
    int exit_code = spawned_process.GetExitCode();
    palPrintf("Exit code = %d\n", exit_code);
    {
      uint64_t len;
      r = ps_stdout.Read(&buff[0], 0, 4096, &len);
      if (r) {
        palBreakHere();
      }
      buff[len] = '\0';
      palPrintf("Read this from spawned process:\n%s\n", buff);
    }
  }

  ps_stdin.Reset();
  ps_stdout.Reset();

  p_stdin.Close();
  p_stdout.Close();
  p_stderr.Close();

  return true;
}

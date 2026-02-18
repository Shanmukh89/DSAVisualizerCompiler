# Ctrl+C Handling Fix

## Problem
Pressing Ctrl+C during CLI visualization was not terminating the program. The program would hang waiting for input and ignore the interrupt signal.

## Solution
Added proper signal handling for Ctrl+C (SIGINT) on both Windows and Unix systems.

### Windows Implementation
- Uses `SetConsoleCtrlHandler()` to register a console control handler
- Handles `CTRL_C_EVENT`, `CTRL_BREAK_EVENT`, and `CTRL_CLOSE_EVENT`
- Sets a global `should_exit` flag when Ctrl+C is pressed
- Checks this flag at key points in the loop

### Unix Implementation
- Uses `signal(SIGINT, signal_handler)` to catch Ctrl+C
- Sets `should_exit` flag when signal is received
- Checks flag before and after blocking I/O operations

## How It Works

1. **Signal Registration**: On program start, registers Ctrl+C handler
2. **Exit Flag**: Uses `volatile int should_exit` to communicate between signal handler and main loop
3. **Check Points**: Checks `should_exit` flag:
   - Before displaying each step
   - Before waiting for user input
   - After user input
   - In the main loop condition

## Usage

When you press **Ctrl+C** during visualization:
```
[Interrupted] Exiting visualization...
[Exiting...]
```

The program will:
- Immediately stop the current step
- Clean up memory (free steps, free data structures)
- Exit gracefully with return code 0

## Testing

To test:
1. Run: `.\dsa_compiler.exe -v test_new_dsl.algo`
2. Wait for the visualization to start
3. Press **Ctrl+C** at any step
4. Program should exit immediately with "[Interrupted] Exiting visualization..."

## Technical Details

### Windows Handler
```c
BOOL WINAPI ConsoleHandler(DWORD dwType) {
    switch (dwType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            should_exit = 1;
            printf("\n\n[Interrupted] Exiting visualization...\n");
            return TRUE;
        default:
            return FALSE;
    }
}
```

### Unix Handler
```c
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        should_exit = 1;
        printf("\n\n[Interrupted] Exiting visualization...\n");
    }
}
```

The `volatile` keyword ensures the flag is checked even when the compiler might optimize it away.


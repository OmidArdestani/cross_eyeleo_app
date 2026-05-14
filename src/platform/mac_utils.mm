#import <AppKit/AppKit.h>

/**
 * Brings the application to the macOS foreground.
 *
 * Qt tray apps run as background agents and are never the "active" process.
 * Without this call, Qt::WindowStaysOnTopHint windows may not appear above
 * other applications and activateWindow() / raise() have no effect.
 */
void mac_activateApplication()
{
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
}

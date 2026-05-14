// installer/config/installcontrol.qs
// Custom controller: brands the Introduction and Finished wizard pages
// with developer contact info and LinkedIn link.
//
// LinkedIn URL: update "omid-ardestani" below to match your actual profile slug.

function Controller() {
    // Automatically accept "target directory already exists" and "existing
    // installation detected" message boxes so that reinstalling over a previous
    // installation works without requiring the user to manually dismiss dialogs
    // or choose a different path.
    //
    // IFW raises different dialogs depending on the state of the target dir:
    //   OverwriteTargetDirectory  – directory exists but no prior IFW metadata
    //   installWarningOverwrite   – general overwrite warning (older IFW builds)
    //   maintenanceToolDetected   – directory contains a maintenance tool/metadata
    //                               from a previous IFW installation (the common
    //                               case on reinstall; this was the missing handler
    //                               that caused reinstalls to fail)
    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("installWarningOverwrite",   QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("maintenanceToolDetected",   QMessageBox.Yes);

    // Set the program to launch after installation (shown as a checkbox on the
    // Finished page).  Must be set early so IFW knows to show RunItCheckBox.
    if (systemInfo.productType === "windows") {
        installer.setValue("RunProgram", "@TargetDir@\\CrossEyeLeoApp.exe");
    } else if (systemInfo.kernelType === "linux") {
        installer.setValue("RunProgram", "@TargetDir@/bin/run_crosseyeleo.sh");
    } else if (systemInfo.productType === "osx") {
        installer.setValue("RunProgram",
            "@TargetDir@/CrossEyeLeoApp.app/Contents/MacOS/CrossEyeLeoApp");
    }
    installer.setValue("RunProgramDescription", "Launch CrossEyeLeoApp now");
}

// ------------------------------------------------------------------
// Helper: normalise a Windows path to use forward slashes.
// IFW on Windows provides paths with forward slashes regardless of what
// the user types, so we normalise before joining to avoid mixed separators.
// On non-Windows platforms this is a no-op.
// ------------------------------------------------------------------
function normalisePath(p) {
    if (systemInfo.productType === "windows") {
        return p.replace(/\\/g, "/");
    }
    return p;
}

// ------------------------------------------------------------------
// Helper: return true if a previous IFW installation of CrossEyeLeoApp
// exists in the given directory.  Detection is based on the presence
// of the maintenance tool binary that IFW places there on first install.
// ------------------------------------------------------------------
function previousInstallationExists(targetDir) {
    if (!targetDir) {
        console.log("[CEL] previousInstallationExists: targetDir is empty/null");
        return false;
    }
    // Normalise to forward slashes so path joins are consistent with how
    // IFW itself reports paths on Windows.
    var dir = normalisePath(targetDir);
    var mtPath;
    if (systemInfo.productType === "windows") {
        mtPath = dir + "/CrossEyeLeoAppMaintenanceTool.exe";
    } else if (systemInfo.productType === "osx") {
        mtPath = dir + "/CrossEyeLeoAppMaintenanceTool.app" +
                       "/Contents/MacOS/CrossEyeLeoAppMaintenanceTool";
    } else {
        mtPath = dir + "/CrossEyeLeoAppMaintenanceTool";
    }
    var exists = installer.fileExists(mtPath);
    console.log("[CEL] previousInstallationExists: checking " + mtPath +
                " -> " + (exists ? "FOUND" : "not found"));
    return exists;
}

// ------------------------------------------------------------------
// Helper: stop any running instance of the application.
// Returns silently whether or not the app was running.
// ------------------------------------------------------------------
function stopRunningApp() {
    console.log("[CEL] stopRunningApp: platform=" + systemInfo.productType);
    try {
        var result;
        if (systemInfo.productType === "windows") {
            console.log("[CEL] stopRunningApp: running taskkill /F /IM CrossEyeLeoApp.exe");
            result = installer.execute("taskkill", ["/F", "/IM", "CrossEyeLeoApp.exe"]);
            console.log("[CEL] stopRunningApp: taskkill exit code=" +
                        (result ? result[0] : "n/a"));
        } else {
            // pkill is available on both Linux and macOS.
            // Exit code 1 (no matching process) is normal and silently ignored.
            console.log("[CEL] stopRunningApp: running pkill -x CrossEyeLeoApp");
            result = installer.execute("pkill", ["-x", "CrossEyeLeoApp"]);
            console.log("[CEL] stopRunningApp: pkill exit code=" +
                        (result ? result[0] : "n/a"));
        }
    } catch (e) {
        // A non-zero exit code (app not running) throws on some IFW builds.
        console.log("[CEL] stopRunningApp: exception (app was likely not running): " + e);
    }
    console.log("[CEL] stopRunningApp: done");
}

// ------------------------------------------------------------------
// Helper: remove the previous installation from targetDir.
//
// Two-step strategy:
//
//   Step 1 – on Linux/macOS only: run the maintenance tool with
//   "--unattended purge" to clean registry entries, shortcuts, and
//   component metadata.  On Windows this step is skipped because IFW
//   holds a lock on cache.lock inside the target directory while the
//   Target Directory page is active; that lock prevents the maintenance
//   tool from running unattended (it blocks on "Do you want to
//   continue?" with no terminal available).  IFW overwrites registry
//   entries and shortcuts during the new installation, so skipping
//   Step 1 on Windows is safe.
//
//   Step 2 – forcefully delete the directory with an OS command.  On
//   Windows we use PowerShell Remove-Item with -ErrorAction
//   SilentlyContinue instead of "rd /S /Q" because the latter aborts
//   completely when it encounters any locked file (such as cache.lock,
//   which IFW holds).  PowerShell skips locked files and deletes
//   everything else; the remaining cache.lock is managed by IFW.
//   On Linux/macOS rm -rf is sufficient.
//
// Both steps are wrapped in try/catch so a first-time install (no
// maintenance tool, no existing directory) silently no-ops.
// ------------------------------------------------------------------
function purgePreviousInstallation(targetDir) {
    console.log("[CEL] purgePreviousInstallation: start, targetDir=" + targetDir +
                ", platform=" + systemInfo.productType);

    if (!targetDir) {
        console.log("[CEL] purgePreviousInstallation: targetDir is empty, aborting");
        return;
    }

    // Normalise to forward slashes (IFW on Windows uses forward slashes).
    var dir = normalisePath(targetDir);
    console.log("[CEL] purgePreviousInstallation: normalised dir=" + dir);

    if (systemInfo.productType === "windows") {
        // Accept both C:/ and C:\ as the path start (IFW always gives C:/).
        if (!/^[A-Za-z]:[/\\]/.test(dir)) {
            console.log("[CEL] purgePreviousInstallation: path does not start with " +
                        "a drive letter, aborting: " + dir);
            return;
        }
    } else {
        if (dir.charAt(0) !== "/") {
            console.log("[CEL] purgePreviousInstallation: path does not start with " +
                        "/, aborting: " + dir);
            return;
        }
    }

    // Step 1: on Linux/macOS run the maintenance tool to remove registry/shortcut
    // metadata.  On Windows this step is intentionally skipped because IFW holds
    // a lock on cache.lock inside the target directory while the Target Directory
    // page is active, which prevents the maintenance tool from running unattended
    // (it blocks on "Do you want to continue?" with no terminal to answer it).
    // IFW will create fresh registry entries and shortcuts during the new
    // installation, so skipping this step on Windows is safe.
    if (systemInfo.productType !== "windows") {
        var mtPath;
        if (systemInfo.productType === "osx") {
            mtPath = dir + "/CrossEyeLeoAppMaintenanceTool.app" +
                           "/Contents/MacOS/CrossEyeLeoAppMaintenanceTool";
        } else {
            mtPath = dir + "/CrossEyeLeoAppMaintenanceTool";
        }
        console.log("[CEL] purgePreviousInstallation: Step 1 - checking for maintenance tool at " +
                    mtPath);
        if (installer.fileExists(mtPath)) {
            console.log("[CEL] purgePreviousInstallation: maintenance tool found, " +
                        "running --unattended purge");
            try {
                var purgeResult = installer.execute(mtPath, ["--unattended", "purge"]);
                console.log("[CEL] purgePreviousInstallation: purge exit code=" +
                            (purgeResult ? purgeResult[0] : "n/a"));
            } catch (e) {
                console.log("[CEL] purgePreviousInstallation: purge threw exception " +
                            "(continuing with directory removal): " + e);
            }
        } else {
            console.log("[CEL] purgePreviousInstallation: maintenance tool not found at " +
                        mtPath + ", skipping Step 1");
        }
    } else {
        console.log("[CEL] purgePreviousInstallation: Step 1 skipped on Windows " +
                    "(IFW cache.lock conflict)");
    }

    // Step 2: forcefully remove the directory so the new installation starts
    // with a completely clean slate.
    //
    // On Windows we use PowerShell's Remove-Item with -ErrorAction SilentlyContinue
    // instead of cmd.exe "rd /S /Q" because:
    //   - IFW holds a lock on cache.lock inside the target directory while the
    //     installer is running.  "rd /S /Q" aborts completely when it hits any
    //     locked file; PowerShell skips locked files and deletes everything else.
    //   - The remaining cache.lock file (~50 bytes) is overwritten/managed by the
    //     new IFW installer during its own installation phase.
    console.log("[CEL] purgePreviousInstallation: Step 2 - forceful directory removal");
    try {
        if (systemInfo.productType === "windows") {
            // Reject characters that would break out of the PowerShell single-quoted
            // string: single quotes, and cmd metacharacters that are also illegal in
            // Windows paths (belt-and-suspenders).
            if (/[&|^!%<>\"']/.test(dir)) {
                console.log("[CEL] purgePreviousInstallation: directory removal skipped - " +
                            "path contains characters unsafe for shell embedding: " + dir);
            } else {
                // Use -LiteralPath so bracket characters in the path name are
                // not treated as wildcard patterns.
                // Single-quoted path prevents variable/expression expansion;
                // single quotes in the path are already rejected above.
                var psCmd = "Remove-Item -LiteralPath '" + dir +
                            "' -Recurse -Force -ErrorAction SilentlyContinue";
                console.log("[CEL] purgePreviousInstallation: running powershell.exe " + psCmd);
                var psResult = installer.execute("powershell.exe",
                    ["-NonInteractive", "-NoProfile", "-Command", psCmd]);
                console.log("[CEL] purgePreviousInstallation: PowerShell exit code=" +
                            (psResult ? psResult[0] : "n/a"));
            }
        } else {
            console.log("[CEL] purgePreviousInstallation: running rm -rf " + dir);
            var rmResult = installer.execute("rm", ["-rf", dir]);
            console.log("[CEL] purgePreviousInstallation: rm exit code=" +
                        (rmResult ? rmResult[0] : "n/a"));
        }
    } catch (e) {
        console.log("[CEL] purgePreviousInstallation: directory removal threw exception: " + e);
    }

    // Verify the directory is actually gone.
    var dirStillExists = installer.fileExists(dir);
    console.log("[CEL] purgePreviousInstallation: done - directory still exists after removal: " +
                dirStillExists);
}

// ------------------------------------------------------------------
// Target directory page
//
// When an existing CrossEyeLeoApp installation is detected at the
// chosen path, the user is asked for confirmation before the running
// app is stopped and the old installation is removed:
//
//   Yes → stop app + purge old installation → user continues normally
//   No  → Next button is disabled; the user must choose a different
//          directory.  As soon as they type a path that does NOT
//          contain a previous installation, Next is re-enabled.
//
// This keeps the stop/remove step at the "Installation Folder" wizard
// page as requested, while always requiring explicit user consent.
// ------------------------------------------------------------------
Controller.prototype.TargetDirectoryPageCallback = function() {
    // Re-affirm IFW's built-in automatic dialog answers (some IFW versions
    // raise these after the constructor fires, e.g. during page validation).
    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("installWarningOverwrite",   QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("maintenanceToolDetected",   QMessageBox.Yes);

    var targetDir = installer.value("TargetDir");
    console.log("[CEL] TargetDirectoryPageCallback: targetDir=" + targetDir);

    if (!previousInstallationExists(targetDir)) {
        console.log("[CEL] TargetDirectoryPageCallback: no existing installation found, " +
                    "first-time install path");
        return;
    }

    console.log("[CEL] TargetDirectoryPageCallback: existing installation detected, " +
                "showing confirmation dialog");

    // An existing installation was found.  Ask the user whether to remove it.
    var result = QMessageBox.question(
        "crosseyeleo.reinstallConfirm",
        "Existing Installation Found",
        "CrossEyeLeoApp is already installed in:\n\n" +
        targetDir + "\n\n" +
        "To install the new version, the running application will be stopped " +
        "and the existing installation will be removed first.\n\n" +
        "Do you want to continue?",
        QMessageBox.Yes | QMessageBox.No
    );

    console.log("[CEL] TargetDirectoryPageCallback: user answered " +
                (result === QMessageBox.Yes ? "Yes" : "No"));

    if (result === QMessageBox.Yes) {
        console.log("[CEL] TargetDirectoryPageCallback: stopping running app...");
        stopRunningApp();
        console.log("[CEL] TargetDirectoryPageCallback: purging previous installation...");
        purgePreviousInstallation(targetDir);
        console.log("[CEL] TargetDirectoryPageCallback: purge complete, " +
                    "user may now click Next");
    } else {
        // User declined.  Disable Next so they must choose a different path.
        console.log("[CEL] TargetDirectoryPageCallback: user declined, disabling Next button");
        var nextBtn = gui.button(buttons.NextButton);
        if (nextBtn) nextBtn.enabled = false;

        // Re-enable Next as soon as the user types a path that has no previous install.
        // Guard against multiple connections if the user navigates back and forth.
        var widget = gui.currentPageWidget();
        if (widget && widget.TargetDirectoryLineEdit && !widget._reinstallListenerAttached) {
            widget._reinstallListenerAttached = true;
            widget.TargetDirectoryLineEdit.textChanged.connect(function(newPath) {
                var hasPrev = previousInstallationExists(newPath);
                console.log("[CEL] TargetDirectoryLineEdit.textChanged: newPath=" + newPath +
                            " hasPreviousInstall=" + hasPrev);
                if (nextBtn) nextBtn.enabled = !hasPrev;
            });
        }
    }
};

// ------------------------------------------------------------------
// Introduction page – show description + contact info
// ------------------------------------------------------------------
Controller.prototype.IntroductionPageCallback = function() {
    var widget = gui.currentPageWidget();
    if (!widget) return;

    var label = widget.MessageLabel;
    if (!label) return;

    label.setOpenExternalLinks(true);
    label.setText(
        "<b>CrossEyeLeo Eye Exercise Reminder</b><br><br>" +
        "A cross-platform application that sits in your system tray and " +
        "periodically prompts you to take short and long breaks with guided " +
        "eye exercises — helping you prevent eye strain during long screen " +
        "sessions.<br><br>" +
        "<b>Developer:</b>&nbsp;Omid Ardestani&nbsp;&nbsp;|&nbsp;&nbsp;" +
        "<a href=\"https://www.linkedin.com/in/omid-ardestani\">LinkedIn</a>" +
        "&nbsp;&nbsp;|&nbsp;&nbsp;" +
        "<a href=\"https://github.com/OmidArdestani/cross_eyeleo_app\">GitHub</a>"
    );
};

// ------------------------------------------------------------------
// Finished page – thank the user and show contact info again
// ------------------------------------------------------------------
Controller.prototype.FinishedPageCallback = function() {
    var widget = gui.currentPageWidget();
    if (!widget) return;

    // Ensure the "launch after installation" checkbox is visible and ticked.
    // The label text comes from RunProgramDescription set in the constructor.
    var runItCheckBox = widget.RunItCheckBox;
    if (runItCheckBox) {
        runItCheckBox.visible = true;
        runItCheckBox.checked = true;
    }

    var label = widget.MessageLabel;
    if (!label) return;

    label.setOpenExternalLinks(true);
    label.setText(
        "CrossEyeLeoApp has been installed successfully!<br><br>" +
        "Thank you for using CrossEyeLeoApp.<br><br>" +
        "<b>Developer:</b>&nbsp;Omid Ardestani<br>" +
        "<a href=\"https://www.linkedin.com/in/omid-ardestani\">" +
        "linkedin.com/in/omid-ardestani</a>&nbsp;&nbsp;|&nbsp;&nbsp;" +
        "<a href=\"https://github.com/OmidArdestani/cross_eyeleo_app\">" +
        "github.com/OmidArdestani/cross_eyeleo_app</a>"
    );
};

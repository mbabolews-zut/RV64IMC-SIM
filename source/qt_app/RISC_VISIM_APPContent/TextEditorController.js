.pragma library

// Rebuilds the model for the UI Repeater
function updateGutterModel(lineCount, activeBreakpoints, listModel) {
    listModel.clear();
    for (var i = 0; i < lineCount; i++) {
        var isBkpt = activeBreakpoints["line_" + i] === true;
        listModel.append({
            "lineNumber": i + 1,
            "hasBreakpoint": isBkpt
        });
    }
}

// Toggles breakpoint state and returns the new map
function toggleBreakpoint(activeBreakpoints, lineIndex) {
    var key = "line_" + lineIndex;
    if (activeBreakpoints[key]) {
        delete activeBreakpoints[key];
    } else {
        activeBreakpoints[key] = true;
    }
    return activeBreakpoints;
}

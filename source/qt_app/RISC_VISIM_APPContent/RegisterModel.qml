import QtQuick

ListModel {
    id: root

    readonly property var regData: [
        { reg: "x0",  abi: "zero" },
        { reg: "x1",  abi: "ra" },
        { reg: "x2",  abi: "sp" },
        { reg: "x3",  abi: "gp" },
        { reg: "x4",  abi: "tp" },
        { reg: "x5",  abi: "t0" },
        { reg: "x6",  abi: "t1" },
        { reg: "x7",  abi: "t2" },
        { reg: "x8",  abi: "s0/fp" },
        { reg: "x9",  abi: "s1" },
        { reg: "x10", abi: "a0" },
        { reg: "x11", abi: "a1" },
        { reg: "x12", abi: "a2" },
        { reg: "x13", abi: "a3" },
        { reg: "x14", abi: "a4" },
        { reg: "x15", abi: "a5" },
        { reg: "x16", abi: "a6" },
        { reg: "x17", abi: "a7" },
        { reg: "x18", abi: "s2" },
        { reg: "x19", abi: "s3" },
        { reg: "x20", abi: "s4" },
        { reg: "x21", abi: "s5" },
        { reg: "x22", abi: "s6" },
        { reg: "x23", abi: "s7" },
        { reg: "x24", abi: "s8" },
        { reg: "x25", abi: "s9" },
        { reg: "x26", abi: "s10" },
        { reg: "x27", abi: "s11" },
        { reg: "x28", abi: "t3" },
        { reg: "x29", abi: "t4" },
        { reg: "x30", abi: "t5" },
        { reg: "x31", abi: "t6" }
    ]

    Component.onCompleted: {
        for (var i = 0; i < regData.length; i++) {
            append({ reg: regData[i].reg, abi: regData[i].abi, value: backend.registers[i] });
        }
    }

    function updateValues() {
        for (var i = 0; i < count; i++) {
            setProperty(i, "value", backend.registers[i]);
        }
    }
}

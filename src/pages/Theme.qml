pragma Singleton
import QtQuick 2.15

QtObject {
    // Base
    readonly property color background: "#020617"   // slate-950
    readonly property color foreground: "#f8fafc"   // slate-50

    // Primary (Gold)
    readonly property color primary: "#eab308"      // gold-500
    readonly property color primaryHover: "#ca8a04" // gold-600

    // Accent (Slate blue-ish like shadcn default)
    readonly property color accent: "#3b82f6"       // blue-500
    readonly property color accentHover: "#2563eb"  // blue-600

    // Secondary surfaces
    readonly property color secondary: "#0f172a"        // slate-900
    readonly property color secondaryHover: "#1e293b"   // slate-800

    // Muted / subtle UI
    readonly property color muted: "#334155"        // slate-700
    readonly property color mutedForeground: "#94a3b8" // slate-400

    // Borders & input
    readonly property color border: "#1e293b"       // slate-800
    readonly property color input: "#020617"        // slate-950

    // States
    readonly property color success: "#22c55e"
    readonly property color warning: "#f59e0b"
    readonly property color error: "#ef4444"
}

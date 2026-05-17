pragma Singleton
import QtQuick 2.15

// QtObject {
//     // Base
//     readonly property color background: "#020617"   // slate-950
//     readonly property color foreground: "#f8fafc"   // slate-50

//     // Primary (Gold)
//     readonly property color primary: "#eab308"      // gold-500
//     readonly property color primaryHover: "#ca8a04" // gold-600

//     // Accent (Slate blue-ish like shadcn default)
//     readonly property color accent: "#3b82f6"       // blue-500
//     readonly property color accentHover: "#2563eb"  // blue-600

//     // Secondary surfaces
//     readonly property color secondary: "#0f172a"        // slate-900
//     readonly property color secondaryHover: "#1e293b"   // slate-800

//     // Muted / subtle UI
//     readonly property color muted: "#334155"        // slate-700
//     readonly property color mutedForeground: "#94a3b8" // slate-400

//     // Borders & input
//     readonly property color border: "#1e293b"       // slate-800
//     readonly property color input: "#020617"        // slate-950

//     // States
//     readonly property color success: "#22c55e"
//     readonly property color warning: "#f59e0b"
//     readonly property color error: "#ef4444"
// }


// pragma Singleton
// import QtQuick 2.15

// QtObject {
//     // =========================================================================
//     // Professional Dark Theme (Gold Accent)
//     // Inspired by modern dashboard/UI systems
//     // =========================================================================

//     // -------------------------------------------------------------------------
//     // Core Backgrounds
//     // -------------------------------------------------------------------------
//     readonly property color background: "#0B1120"          // Deep navy-black
//     readonly property color backgroundElevated: "#111827" // Elevated panels
//     readonly property color surface: "#0F172A"            // Main card surface
//     readonly property color surfaceHover: "#172033"       // Hovered cards

//     // -------------------------------------------------------------------------
//     // Typography
//     // -------------------------------------------------------------------------
//     readonly property color foreground: "#F8FAFC"         // Primary text
//     readonly property color foregroundMuted: "#CBD5E1"    // Secondary text
//     readonly property color foregroundSubtle: "#94A3B8"   // Tertiary text

//     // -------------------------------------------------------------------------
//     // Primary Brand (Luxury Gold)
//     // -------------------------------------------------------------------------
//     readonly property color primary: "#FACC15"            // Rich gold
//     readonly property color primaryHover: "#EAB308"
//     readonly property color primaryPressed: "#CA8A04"

//     // Gold glow / focus effects
//     readonly property color primarySoft: "#3B2F05"
//     readonly property color primaryBorder: "#7C5A10"

//     // -------------------------------------------------------------------------
//     // Accent
//     // -------------------------------------------------------------------------
//     readonly property color accent: "#3B82F6"
//     readonly property color accentHover: "#2563EB"
//     readonly property color accentSoft: "#172554"

//     // -------------------------------------------------------------------------
//     // Secondary UI
//     // -------------------------------------------------------------------------
//     readonly property color secondary: "#1E293B"
//     readonly property color secondaryHover: "#273449"

//     // -------------------------------------------------------------------------
//     // Inputs / Interactive
//     // -------------------------------------------------------------------------
//     readonly property color input: "#111827"
//     readonly property color inputHover: "#172033"
//     readonly property color inputFocus: "#1E293B"

//     // -------------------------------------------------------------------------
//     // Borders / Dividers
//     // -------------------------------------------------------------------------
//     readonly property color border: "#243041"
//     readonly property color borderSoft: "#1B2433"
//     readonly property color divider: "#1E293B"

//     // -------------------------------------------------------------------------
//     // Muted Elements
//     // -------------------------------------------------------------------------
//     readonly property color muted: "#334155"
//     readonly property color mutedForeground: "#94A3B8"

//     // -------------------------------------------------------------------------
//     // Status Colors
//     // -------------------------------------------------------------------------
//     readonly property color success: "#22C55E"
//     readonly property color successSoft: "#052E16"

//     readonly property color warning: "#F59E0B"
//     readonly property color warningSoft: "#3B2204"

//     readonly property color error: "#EF4444"
//     readonly property color errorSoft: "#3B0A0A"

//     readonly property color info: "#38BDF8"
//     readonly property color infoSoft: "#082F49"

//     // -------------------------------------------------------------------------
//     // Overlays / Effects
//     // -------------------------------------------------------------------------
//     readonly property color overlay: "#AA020617"
//     readonly property color shadow: "#000000"

//     // -------------------------------------------------------------------------
//     // Selection / Highlight
//     // -------------------------------------------------------------------------
//     readonly property color selection: "#7C5A10"
//     readonly property color selectionForeground: "#FFFFFF"
// }


QtObject {
    // =========================================================================
    // Professional Minimal Dark Theme (Gold Accent)
    // Black → Off-white grayscale with luxury gold highlights
    // =========================================================================

    // -------------------------------------------------------------------------
    // Core Backgrounds
    // -------------------------------------------------------------------------
    readonly property color background: "#0A0A0A"          // Pure dark base
    readonly property color backgroundElevated: "#111111" // Elevated surfaces
    readonly property color surface: "#161616"            // Cards / containers
    readonly property color surfaceHover: "#1E1E1E"       // Hover state

    // -------------------------------------------------------------------------
    // Typography
    // -------------------------------------------------------------------------
    readonly property color foreground: "#F5F5F5"         // Main text
    readonly property color foregroundMuted: "#D4D4D4"    // Secondary text
    readonly property color foregroundSubtle: "#A3A3A3"   // Tertiary text

    // -------------------------------------------------------------------------
    // Primary Brand (Luxury Gold)
    // KEEPING GOLDEN THEME
    // -------------------------------------------------------------------------
    readonly property color primary: "#FACC15"
    readonly property color primaryHover: "#EAB308"
    readonly property color primaryPressed: "#CA8A04"

    // Gold glow / focus effects
    readonly property color primarySoft: "#3B2F05"
    readonly property color primaryBorder: "#7C5A10"

    // -------------------------------------------------------------------------
    // Accent
    // -------------------------------------------------------------------------
    readonly property color accent: "#D4D4D4"
    readonly property color accentHover: "#E5E5E5"
    readonly property color accentSoft: "#262626"

    // -------------------------------------------------------------------------
    // Secondary UI
    // -------------------------------------------------------------------------
    readonly property color secondary: "#202020"
    readonly property color secondaryHover: "#2A2A2A"

    // -------------------------------------------------------------------------
    // Inputs / Interactive
    // -------------------------------------------------------------------------
    readonly property color input: "#141414"
    readonly property color inputHover: "#1D1D1D"
    readonly property color inputFocus: "#262626"

    // -------------------------------------------------------------------------
    // Borders / Dividers
    // -------------------------------------------------------------------------
    readonly property color border: "#303030"
    readonly property color borderSoft: "#242424"
    readonly property color divider: "#2A2A2A"

    // -------------------------------------------------------------------------
    // Muted Elements
    // -------------------------------------------------------------------------
    readonly property color muted: "#404040"
    readonly property color mutedForeground: "#A3A3A3"

    // -------------------------------------------------------------------------
    // Status Colors
    // KEEPING STATUS COLORS
    // -------------------------------------------------------------------------
    readonly property color success: "#22C55E"
    readonly property color successSoft: "#052E16"

    readonly property color warning: "#F59E0B"
    readonly property color warningSoft: "#3B2204"

    readonly property color error: "#EF4444"
    readonly property color errorSoft: "#3B0A0A"

    readonly property color info: "#38BDF8"
    readonly property color infoSoft: "#082F49"

    // -------------------------------------------------------------------------
    // Overlays / Effects
    // -------------------------------------------------------------------------
    readonly property color overlay: "#AA000000"
    readonly property color shadow: "#000000"

    // -------------------------------------------------------------------------
    // Selection / Highlight
    // -------------------------------------------------------------------------
    readonly property color selection: "#7C5A10"
    readonly property color selectionForeground: "#FFFFFF"
}
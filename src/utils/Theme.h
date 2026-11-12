#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Centralized theme constants (colors, radii, font sizes, dimensions) using SFML.
 */
namespace Theme {
    // Window dimensions
    constexpr float WindowWidth = 1400.0f;
    constexpr float WindowHeight = 800.0f;
    constexpr float PanelWidth = 320.0f;

    // Colors
    inline const sf::Color kWindowBg(13, 27, 42);           // #0D1B2A
    inline const sf::Color kPanelBg(30, 42, 56);            // #1E2A38
    inline const sf::Color kPanelSectionBg(36, 52, 71);     // #243447

    // Aliases for compatibility
    inline const sf::Color ColorWindowBg = kWindowBg;
    inline const sf::Color ColorPanelBg = kPanelBg;
    inline const sf::Color ColorSectionBg = kPanelSectionBg;

    // Node colors
    inline const sf::Color kNodeDefault(91, 107, 122);      // #5B6B7A (slate grey)
    inline const sf::Color kNodeSource(46, 204, 113);       // #2ECC71 (emerald green)
    inline const sf::Color kNodeDest(231, 76, 60);          // #E74C3C (crimson red)
    inline const sf::Color kNodeVisiting(241, 196, 15);     // #F1C40F (yellow)
    inline const sf::Color kNodeVisited(39, 174, 96);       // #27AE60 (green)
    inline const sf::Color kNodeUnvisited(52, 73, 94);      // #34495E (dimmed)

    inline const sf::Color ColorNodeDefault = kNodeDefault;
    inline const sf::Color ColorNodeSource = kNodeSource;
    inline const sf::Color ColorNodeDest = kNodeDest;
    inline const sf::Color ColorNodeVisiting = kNodeVisiting;
    inline const sf::Color ColorNodeVisited = kNodeVisited;
    inline const sf::Color ColorNodeUnvisited = kNodeUnvisited;

    // Edge colors
    inline const sf::Color kEdgeDefault(74, 85, 104);       // #4A5568
    inline const sf::Color kEdgePath(231, 76, 60);          // #E74C3C (thick red)
    inline const sf::Color kEdgePathGlow(255, 107, 107, 100); // Translucent glow

    inline const sf::Color ColorEdgeDefault = kEdgeDefault;
    inline const sf::Color ColorEdgePath = kEdgePath;
    inline const sf::Color ColorEdgePathGlow = kEdgePathGlow;

    // Vehicle
    inline const sf::Color kVehicle(0, 255, 255);           // Cyan glow
    inline const sf::Color kVehicleGlow(0, 255, 255, 100);

    inline const sf::Color ColorVehicle = kVehicle;
    inline const sf::Color ColorVehicleGlow = kVehicleGlow;

    // Text colors
    inline const sf::Color kTextWhite(255, 255, 255);
    inline const sf::Color kTextMuted(136, 153, 170);       // #8899AA
    inline const sf::Color kTextActive(46, 204, 113);

    inline const sf::Color ColorTextWhite = kTextWhite;
    inline const sf::Color ColorTextMuted = kTextMuted;
    inline const sf::Color ColorTextActive = kTextActive;
    inline const sf::Color ColorActive = kTextActive;

    // Sizes & Radii
    constexpr float kNodeRadiusDefault = 22.0f;
    constexpr float kNodeRadiusHover = 28.0f;
    constexpr float kVehicleRadius = 10.0f;

    constexpr float kEdgeWidthDefault = 2.0f;
    constexpr float kEdgeWidthPath = 5.0f;

    constexpr unsigned int kFontSizeTitle = 22;
    constexpr unsigned int kFontSizeHeading = 16;
    constexpr unsigned int kFontSizeBody = 13;
    constexpr unsigned int kFontSizeSmall = 11;

    constexpr unsigned int FontSizeTitle = kFontSizeTitle;
    constexpr unsigned int FontSizeHeading = kFontSizeHeading;
    constexpr unsigned int FontSizeBody = kFontSizeBody;
    constexpr unsigned int FontSizeSmall = kFontSizeSmall;
}

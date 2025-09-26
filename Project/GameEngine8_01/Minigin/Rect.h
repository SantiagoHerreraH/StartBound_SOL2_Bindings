#pragma once

#include "../3rdParty/glm/glm.hpp"
#include <algorithm>
#include <windows.h>
#undef min
#undef max

namespace dae {

    //Bottom > Top because +y is down
    struct Rect {
        int Top, Left;      // Bottom-left corner
        int Width, Height;

        int Right() const { return Left + Width; }
        int Bottom() const { return Top + Height; } 
    };

    inline bool IsColliding(const Rect& a, const Rect& b) {
        return (
            a.Right() > b.Left &&  
            a.Left < b.Right() &&  
            a.Top < b.Bottom() &&  
            a.Bottom() > b.Top);
    }

    inline bool ResolveCollision(Rect& a, glm::vec2& velA, Rect& b, glm::vec2& velB) {
        // Predict future positions
        int aNextLeft = a.Left + velA.x;
        int aNextTop = a.Top + velA.y;
        int aNextRight = aNextLeft + a.Width;
        int aNextBottom = aNextTop + a.Height;

        int bNextLeft = b.Left + velB.x;
        int bNextTop = b.Top + velB.y;
        int bNextRight = bNextLeft + b.Width;
        int bNextBottom = bNextTop + b.Height;

        // Check for overlap
        if (aNextRight <= bNextLeft || aNextLeft >= bNextRight ||
            aNextBottom <= bNextTop || aNextTop >= bNextBottom) {
            return false; // No collision
        }

        // Calculate overlaps
        int overlapX = std::min(aNextRight, bNextRight) - std::max(aNextLeft, bNextLeft);
        int overlapY = std::min(aNextBottom, bNextBottom) - std::max(aNextTop, bNextTop);

        // Resolve on the axis of least penetration
        if (overlapX < overlapY) {
            // Push apart along X
            if (aNextLeft < bNextLeft) {
                a.Left -= overlapX / 2;
                b.Left += overlapX / 2;

                // Clamp only if moving into each other
                if (velA.x > 0) velA.x = 0;
                if (velB.x < 0) velB.x = 0;
            }
            else {
                a.Left += overlapX / 2;
                b.Left -= overlapX / 2;

                if (velA.x < 0) velA.x = 0;
                if (velB.x > 0) velB.x = 0;
            }
        }
        else {
            // Push apart along Y
            if (aNextTop < bNextTop) {
                a.Top -= overlapY / 2;
                b.Top += overlapY / 2;

                if (velA.y > 0) velA.y = 0;
                if (velB.y < 0) velB.y = 0;
            }
            else {
                a.Top += overlapY / 2;
                b.Top -= overlapY / 2;

                if (velA.y < 0) velA.y = 0;
                if (velB.y > 0) velB.y = 0;
            }
        }

        return true;
    }

}


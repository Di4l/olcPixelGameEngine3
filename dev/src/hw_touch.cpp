#include "hw_touch.h"

//! START IMPLEMENTATION
namespace olc::hw
{
    const std::vector<uint32_t>& Touch::GetTouchIDs() const
    {
        static std::vector<uint32_t> touchIDs;
        touchIDs.clear();
        for (const auto& [id, index] : touches_cache)
            touchIDs.push_back(id);
        return touchIDs;
    }

    const bool Touch::IsTouch(const uint32_t nID) const
    {
        return touches_cache.contains(nID);
    }

    const TouchPoint& Touch::GetTouch(const uint32_t nID) const
    {
        return touches_cache.at(nID);
    }

    void Touch::UpdateTouch(const uint32_t nID, const olc::vf2d& vPos, const bool bPress, const bool bRelease, const olc::vf2d& vSize, const bool stylus, const float pressure, const float orientation, const olc::vf2d& tilt)
    {
        // If touch doesnt exist, this will create one or
        // it will update an existing one
        touches_live[nID].position = vPos;
        touches_live[nID].size = vSize;
		touches_live[nID].bPressed = bPress || touches_live[nID].bPressed;
		touches_live[nID].bReleased = bRelease;
		touches_live[nID].bHeld = !bRelease;
		touches_live[nID].pressure = pressure;
		touches_live[nID].orientation = orientation;
		touches_live[nID].tilt = tilt;
		touches_live[nID].bStylus = stylus;
    }

    void Touch::UpdateState()
    {
        touches_cache = touches_live;

		// Remove released touches from live state
        for (auto it = touches_live.begin(); it != touches_live.end(); )
        {
			if (it->second.bPressed)
				it->second.bPressed = false;

            if (it->second.bReleased)
                it = touches_live.erase(it);
            else
                ++it;
        }
    }
}
//! END IMPLEMENTATION

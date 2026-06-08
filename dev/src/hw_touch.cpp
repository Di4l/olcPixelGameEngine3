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

    void Touch::UpdateTouch(const uint32_t nID, const olc::vf2d& vPos, const bool bPress, const bool bRelease, const olc::vf2d& vSize)
    {
        //      // Does touch already exist?
              //if (touches_live.contains(nID))
              //{
                  // It does, so update existing touch

            touches_live[nID].position = vPos;
            touches_live[nID].size = vSize;
			touches_live[nID].bPressed = bPress || touches_live[nID].bPressed;
			touches_live[nID].bReleased = bRelease;
			touches_live[nID].bHeld = !bRelease;
		//}
  //      else
  //      {
		//	// Touch doesnt exist
  //          if (bPress)
  //          {
  //              // Add new touch
  //              touches_live[nID] = { vPos, vSize };
  //          }
        //}
    }

    void Touch::UpdateState()
    {
  //      // Copy over the "live" touch state. Any that are missing from this, but exist in the
  //      // cached state, will remain cached for one more update, but with the released flag set

		//for (const auto& [id, touch] : touches_cache)
		//{
		//	if (!touches_live.contains(id) && !touch.bReleased)
		//	{
		//		// Touch was released, but we want to keep it around for one more update so we can report the release event
  //              touches_live[id] = touch;
  //              touches_live[id].bReleased = true;
  //              touches_live[id].bHeld = false;
		//	}
		//}

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

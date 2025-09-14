#include "hw_mouse.h"

//! START IMPLEMENTATION
namespace olc::hw
{
    const Button& Mouse::GetButton(const int nButton) const
    {
        return buttons.at(nButton);
    }

    const olc::vf2d& Mouse::GetPosition() const
    {
        return position;
    }

    void Mouse::SetPosition(const olc::vf2d& pos)
    {
        position_in = pos;
    }

    void Mouse::SetButton(const int nButton, bool state)
    {        
        buttons_new[nButton] = state;            
    }

    void Mouse::UpdateState()
    {
        for (size_t i=0; i<buttons.size(); i++)
        {
            buttons[i].bPressed = false;
            buttons[i].bReleased = false;
            if (buttons_new[i] != buttons_old[i])
            {
                if (buttons_new[i])
                {
                    buttons[i].bPressed = !buttons[i].bHeld;
                    buttons[i].bHeld = true;
                }
                else
                {
                    buttons[i].bReleased = true;
                    buttons[i].bHeld = false;
                }
            }
            buttons_old[i] = buttons_new[i];
        }

        position = position_in;
    }
}
//! END IMPLEMENTATION

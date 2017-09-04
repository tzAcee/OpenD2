#pragma once
#include "D2Menu.hpp"

class D2Menu_Trademark : public D2Menu
{
private:
	tex_handle backgroundTexture;
	anim_handle flameLeftAnim;
	anim_handle flameRightAnim;
	anim_handle blackLeftAnim;
	anim_handle blackRightAnim;

public:
	D2Menu_Trademark();
	~D2Menu_Trademark();

	virtual void Draw();
};

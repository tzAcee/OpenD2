#include "D2Widget_CharSelectList.hpp"
#include "D2Panel_CharSelect.hpp"
#include "D2Widget_CharSelectSave.hpp"

// Mappings for the class token
static char* gszClassTokens[D2CLASS_MAX] = {
	"AM", "SO", "NE", "PA", "BA", "DZ", "AI",
};

/*
 *	Creates a Character Select list widget.
 *	This method is responsible for loading up all of the savegames.
 *	We should maybe cache the results of the savegame loading so that going to the charselect page doesn't take a while.
 */
D2Widget_CharSelectList::D2Widget_CharSelectList(int x, int y, int w, int h, IRenderObject* renderedName) 
	: D2Widget(x, y, w, h)
{
	// Blank out our own data
	nNumberSaves = 0;
	nCurrentScroll = 0;
	nCurrentSelection = -1;
	saves = nullptr;
	pCharacterData = nullptr;

	greyFrameRef = engine->graphics->CreateReference("data\\global\\ui\\CharSelect\\charselectboxgrey.dc6", UsagePolicy_Permanent);
	frameRef = engine->graphics->CreateReference("data\\global\\ui\\CharSelect\\charselectbox.dc6", UsagePolicy_Permanent);

	// Create the scrollbar - we manually draw it as part of this widget's display
	//pScrollBar = new D2Widget_Scrollbar()

	topName = renderedName;
}

/*
 *	Destroys the character select list widget
 */
D2Widget_CharSelectList::~D2Widget_CharSelectList()
{
	// Free out the entire linked list
	if (saves)
	{
		delete saves;
	}

	engine->graphics->DeleteReference(greyFrameRef);
	engine->graphics->DeleteReference(frameRef);
}

/*
 *	Add a savegame to the list
 */
void D2Widget_CharSelectList::AddSave(D2SaveHeader& header, char* path)
{
	// Allocate a character save entry
	D2Widget_CharSelectSave* newSave = new D2Widget_CharSelectSave(path, header);
	
	newSave->SetNextInChain(saves);
	saves = newSave;

	// Increment the save count.
	nNumberSaves++;
}

/*
 *	This widget got added to the panel. Let's go ahead and tell the parent what we have selected.
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::OnWidgetAdded()
{
	Selected(nCurrentSelection);
}

/*
 *	Draws a Character Select list widget.
 */
void D2Widget_CharSelectList::Draw()
{
	// Draw the savegames
	saves->GetInChain(nCurrentScroll)->DrawLink(D2_NUM_VISIBLE_SAVES, true);
}
/*
 *	Returns the name of the currently selected character.
 *	@author	eezstreet
 */
char16_t* D2Widget_CharSelectList::GetSelectedCharacterName()
{
	if (saves)
	{
		return saves->GetSelectedCharacterName();
	}

	return u"";
}

/*
 *	Handles a mouse-down event on a CharSelectList widget.
 */
bool D2Widget_CharSelectList::HandleMouseDown(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		return true;
	}
	return false;
}

/*
 *	Handles a mouse click event on a CharSelectList widget.
 */
bool D2Widget_CharSelectList::HandleMouseClick(DWORD dwX, DWORD dwY)
{
	if (dwX >= x && dwX <= x + w && dwY >= y && dwY <= y + h)
	{
		Clicked(dwX - x, dwY - y);
		return true;
	}
	return false;
}

/*
 *	Handles a click in a relative area
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::Clicked(DWORD dwX, DWORD dwY)
{
	bool bClickedRight = dwX > (w / 2);
	int nClickY = dwY / (h / 4);
	int nClickSlot = (nClickY * 2) + bClickedRight;
	int nNewSelection = nClickSlot + nCurrentScroll;

	if (nNewSelection >= nNumberSaves)
	{
		nNewSelection = -1;
	}

	nCurrentSelection = nNewSelection;
	Selected(nCurrentSelection);
}

/*
 *	A new character slot was selected
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::Selected(int nNewSelection)
{
	if (nNewSelection >= nNumberSaves || nNewSelection + nCurrentScroll >= nNumberSaves)
	{
		nNewSelection = -1;
	}

	if (saves == nullptr)
	{
		nNewSelection = -1;
	}

	if (saves != nullptr)
	{
		saves->DeselectAllInChain();
	}

	if (nNewSelection == -1)
	{
		// Grey out the "OK", "Delete Character" and "Convert to Expansion" buttons
		dynamic_cast<D2Panel_CharSelect*>(m_pOwner)->InvalidateSelection();
		nCurrentSelection = nNewSelection;
		topName->SetText(u"");
	}
	else
	{
		dynamic_cast<D2Panel_CharSelect*>(m_pOwner)->ValidateSelection();
		nCurrentSelection = nCurrentScroll + nNewSelection;
		if (saves != nullptr)
		{
			saves->Select(nCurrentSelection);
			topName->SetText(saves->GetSelectedCharacterName());
		}
	}
}

/*
 *	We need to load up the selected save.
 *	@author	eezstreet
 */
void D2Widget_CharSelectList::LoadSave()
{
	CharacterSaveData* pCurrent;

	if (nCurrentSelection == -1)
	{
		return;
	}

	// Advance nCurrentSelection times through the linked list
	pCurrent = pCharacterData;
	for (int i = 0; i < nCurrentSelection && pCurrent != nullptr; i++, pCurrent = pCurrent->pNext);

	if (pCurrent == nullptr)
	{	// invalid selection
		return;
	}

	memcpy(&cl.currentSave.header, &pCurrent->header, sizeof(pCurrent->header));
	D2Lib::strncpyz(cl.szCurrentSave, pCurrent->path, MAX_D2PATH_ABSOLUTE);
}
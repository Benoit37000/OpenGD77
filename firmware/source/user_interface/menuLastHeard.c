/*
 * Copyright (C)2019 Roger Clark. VK3KYY / G4KYF
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <user_interface/menuSystem.h>
#include <user_interface/uiUtilityQSOData.h>
#include <user_interface/uiLocalisation.h>
#include <functions/fw_ticks.h>

const int LAST_HEARD_NUM_LINES_ON_DISPLAY = 3;
static bool displayLHDetails = false;

static void handleEvent(uiEvent_t *ev);
static void menuLastHeardDisplayTA(uint8_t y, char *text, uint32_t time, size_t maxLen, bool displayDetails);

int menuLastHeard(uiEvent_t *ev, bool isFirstRun)
{
	if (isFirstRun)
	{
		gMenusStartIndex = LinkHead->id;// reuse this global to store the ID of the first item in the list
		gMenusEndIndex=0;
		displayLHDetails = false;
		menuLastHeardUpdateScreen(true, displayLHDetails);
	}
	else
	{
		// do live update by checking if the item at the top of the list has changed
		if (gMenusStartIndex != LinkHead->id || menuDisplayQSODataState==QSO_DISPLAY_CALLER_DATA)
		{
			gMenusStartIndex = LinkHead->id;
			gMenusCurrentItemIndex=0;
			gMenusEndIndex=0;
			menuLastHeardUpdateScreen(true, displayLHDetails);
		}

		if (ev->hasEvent)
			handleEvent(ev);
	}
	return 0;
}

void menuLastHeardUpdateScreen(bool showTitleOrHeader, bool displayDetails)
{
	static const int bufferLen = 17;
	char buffer[bufferLen];
	dmrIdDataStruct_t foundRecord;
	int numDisplayed = 0;
	LinkItem_t *item = LinkHead;

	ucClearBuf();
	if (showTitleOrHeader)
	{
		menuDisplayTitle(currentLanguage->last_heard);
	}
	else
	{
		menuUtilityRenderHeader();
	}

	// skip over the first gMenusCurrentItemIndex in the listing
	for(int i = 0; i < gMenusCurrentItemIndex; i++)
	{
		item = item->next;
	}

	while((item != NULL) && item->id != 0)
	{
		if (dmrIDLookup(item->id, &foundRecord))
		{
			menuLastHeardDisplayTA(16 + (numDisplayed * 16), foundRecord.text, item->time, 20, displayDetails);
			//ucPrintCentered(16 + (numDisplayed * 16), foundRecord.text, FONT_8x16);
		}
		else
		{
			if (item->talkerAlias[0] != 0x00)
			{
				menuLastHeardDisplayTA(16 + (numDisplayed * 16), item->talkerAlias, item->time, 32, displayDetails);
				//memcpy(buffer, item->talkerAlias, bufferLen - 1);// limit to 1 line of the display which is 16 chars at the normal font size
			}
			else
			{
				snprintf(buffer, bufferLen, "ID:%d", item->id);
				buffer[bufferLen - 1] = 0;
				menuLastHeardDisplayTA(16 + (numDisplayed * 16), buffer, item->time, bufferLen, displayDetails);
				//ucPrintCentered(16 + (numDisplayed * 16), buffer, FONT_8x16);
			}
		}

		numDisplayed++;

		item = item->next;
		if (numDisplayed > (LAST_HEARD_NUM_LINES_ON_DISPLAY - 1))
		{
			if (item != NULL && item->id != 0)
			{
				gMenusEndIndex = 0x01;
			}
			else
			{
				gMenusEndIndex = 0;
			}
			break;
		}
	}

	ucRender();
	menuDisplayQSODataState = QSO_DISPLAY_IDLE;
}

static void handleEvent(uiEvent_t *ev)
{
	displayLightTrigger();

	if (KEYCHECK_PRESS(ev->keys,KEY_DOWN) && gMenusEndIndex!=0)
	{
		gMenusCurrentItemIndex++;
	}
	else if (KEYCHECK_PRESS(ev->keys,KEY_UP))
	{
		gMenusCurrentItemIndex--;
		if (gMenusCurrentItemIndex<0)
		{
			gMenusCurrentItemIndex=0;
		}
	}
	else if (KEYCHECK_SHORTUP(ev->keys,KEY_RED))
	{
		menuSystemPopPreviousMenu();
		return;
	}
	else if (KEYCHECK_SHORTUP(ev->keys,KEY_GREEN))
	{
		menuSystemPopAllAndDisplayRootMenu();
		return;
	}

	// Display Last Heard details while SK1 is pressed
	if ((displayLHDetails == false) && (ev->buttons == BUTTON_SK1))
	{
		displayLHDetails = true;
		menuLastHeardUpdateScreen(true, displayLHDetails);
		return;
	}
	else if (displayLHDetails && ((ev->buttons & BUTTON_SK1) == 0))
	{
		displayLHDetails = false;
		menuLastHeardUpdateScreen(true, displayLHDetails);
		return;
	}

	menuLastHeardUpdateScreen(true, false);
}

static void menuLastHeardDisplayTA(uint8_t y, char *text, uint32_t time, size_t maxLen, bool displayDetails)
{
	char buffer[37]; // Max: TA 27 (in 7bit format) + ' [' + 6 (Maidenhead)  + ']' + NULL

	// Display elapsed time
	if (displayDetails)
	{

		char buf[10]; // hhh:mm:ss
		uint32_t diffTimeInSecs = ((fw_millis() - time) / 1000U);
		uint16_t h = (diffTimeInSecs / 3600);
		uint16_t m = (diffTimeInSecs - (3600 * h)) / 60;
		uint16_t s = (diffTimeInSecs - (3600 * h) - (m * 60));
		snprintf(buf, 10, "%02d:%02d:%02d", h, m, s);
		buf[9] = 0;

		ucPrintAt(((128 - (8 * 6)) - 4), y, buf, FONT_6x8_BOLD);

	}

	if (strlen(text) >= 5)
	{
		char    *pbuf;
		int32_t  cpos;

		if ((cpos = getCallsignEndingPos(text)) != -1)
		{
			// Callsign found
			memcpy(buffer, text, cpos);
			buffer[cpos] = 0;

			if (displayDetails == false)
			{
				ucPrintCentered(y, chomp(buffer), FONT_8x16);
			}
			else
			{
				ucPrintAt(4, y, chomp(buffer), FONT_6x8_BOLD);

				memcpy(buffer, text + (cpos + 1), (maxLen - (cpos + 1)));
				buffer[(strlen(text) - (cpos + 1))] = 0;

				pbuf = chomp(buffer);

				if (strlen(pbuf))
					printSplitOrSpanText(y + 8, pbuf, true);
			}
		}
		else
		{
			// No space found, use a chainsaw
			memcpy(buffer, text, 16);
			buffer[16] = 0;

			if (displayDetails == false)
			{
				ucPrintCentered(y, chomp(buffer), FONT_8x16);
			}
			else
			{
				ucPrintAt(4, y, chomp(buffer), FONT_6x8_BOLD);

				memcpy(buffer, text + 16, (maxLen - 16));
				buffer[(strlen(text) - 16)] = 0;

				pbuf = chomp(buffer);

				if (strlen(pbuf))
					printSplitOrSpanText(y + 8, pbuf, true);
			}
		}
	}
	else
	{
		memcpy(buffer, text, strlen(text));
		buffer[strlen(text)] = 0;

		if (displayDetails == false)
		{
			ucPrintCentered(y, chomp(buffer), FONT_8x16);
		}
		else
		{
			ucPrintAt(4, y, chomp(buffer), FONT_6x8_BOLD);
		}
	}
}

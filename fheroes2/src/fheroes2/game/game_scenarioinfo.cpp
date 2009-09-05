/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <algorithm>
#include <vector>
#include <string>
#include "agg.h"
#include "gamedefs.h"
#include "button.h"
#include "cursor.h"
#include "settings.h"
#include "maps_fileinfo.h"
#include "difficulty.h"
#include "dialog.h"
#include "text.h"
#include "kingdom.h"
#include "splitter.h"
#include "world.h"
#include "player.h"
#include "game.h"

extern bool DialogSelectMapsFileList(MapsFileInfoList &, std::string &);
extern bool PrepareMapsFileInfoList(MapsFileInfoList &);

u16 GetStepFor(u16, u16, u16);
void RedrawRatingInfo(TextSprite &);
void UpdateCoordOpponentsInfo(const Point &, std::vector<Rect> &);
void UpdateCoordClassInfo(const Point &, std::vector<Rect> &);

Game::menu_t Game::ScenarioInfo(void)
{
    AGG::PlayMusic(MUS::MAINMENU);

    Settings & conf = Settings::Get();

    MapsFileInfoList lists;
    if(!PrepareMapsFileInfoList(lists)) return MAINMENU;

    // preload
    AGG::PreloadObject(ICN::HEROES);
    AGG::PreloadObject(ICN::NGEXTRA);
    AGG::PreloadObject(ICN::NGHSBKG);

    menu_t result = QUITGAME;
    LocalEvent & le = LocalEvent::Get();

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    cursor.SetThemes(cursor.POINTER);

    Display & display = Display::Get();
    display.SetVideoMode(640, 480);

    // set first maps settings
    conf.LoadFileMaps(lists.front().file);

    const Point pointPanel(204, 32);
    const Point pointOpponentInfo(pointPanel.x + 24, pointPanel.y + 202);
    const Point pointClassInfo(pointPanel.x + 24, pointPanel.y + 282);

    const Sprite &panel = AGG::GetICN(ICN::NGHSBKG, 0);
    const Rect box(pointPanel, panel.w(), panel.h());

    Button buttonSelectMaps(pointPanel.x + 309, pointPanel.y + 45, ICN::NGEXTRA, 64, 65);
    Button buttonOk(pointPanel.x + 31, pointPanel.y + 380, ICN::NGEXTRA, 66, 67);
    Button buttonCancel(pointPanel.x + 287, pointPanel.y + 380, ICN::NGEXTRA, 68, 69);

    std::vector<Rect>::iterator itr;

    // vector coord difficulty
    std::vector<Rect> coordDifficulty(5);

    // vector coord colors opponent
    std::vector<Rect> coordColors(KINGDOMMAX);

    // vector coord class
    std::vector<Rect> coordClass(KINGDOMMAX);

    // first allow color
    conf.SetMyColor(conf.FirstAllowColor());
    conf.SetPlayersColors(conf.MyColor());

    // image background
    const Sprite &back = AGG::GetICN(ICN::HEROES, 0);
    display.Blit(back);

    Scenario::RedrawStaticInfo(pointPanel);

    UpdateCoordOpponentsInfo(pointOpponentInfo, coordColors);
    Scenario::RedrawOpponentsInfo(pointOpponentInfo);

    UpdateCoordClassInfo(pointClassInfo, coordClass);
    Scenario::RedrawClassInfo(pointClassInfo);

    const Point pointDifficultyNormal(pointPanel.x + 98, pointPanel.y + 92);
    SpriteCursor levelCursor(AGG::GetICN(ICN::NGEXTRA, 62), pointDifficultyNormal);
    levelCursor.Show(pointDifficultyNormal);
    conf.SetGameDifficulty(Difficulty::NORMAL);

    coordDifficulty[0] = Rect(pointPanel.x + 21, pointPanel.y + 92,  levelCursor.w(), levelCursor.h());
    coordDifficulty[1] = Rect(pointDifficultyNormal.x, pointDifficultyNormal.y,  levelCursor.w(), levelCursor.h());
    coordDifficulty[2] = Rect(pointPanel.x + 174, pointPanel.y + 92, levelCursor.w(), levelCursor.h());
    coordDifficulty[3] = Rect(pointPanel.x + 251, pointPanel.y + 92, levelCursor.w(), levelCursor.h());
    coordDifficulty[4] = Rect(pointPanel.x + 328, pointPanel.y + 92, levelCursor.w(), levelCursor.h());


    TextSprite rating;
    rating.SetFont(Font::BIG);
    rating.SetPos(pointPanel.x + 166, pointPanel.y + 383);
    RedrawRatingInfo(rating);

    buttonSelectMaps.Draw();
    buttonOk.Draw();
    buttonCancel.Draw();

    cursor.Show();
    display.Flip();

    while(le.HandleEvents())
    {
	// press button
	le.MousePressLeft(buttonSelectMaps) ? buttonSelectMaps.PressDraw() : buttonSelectMaps.ReleaseDraw();
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
	le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

	// click select
	if(le.KeyPress(KEY_s) || le.MouseClickLeft(buttonSelectMaps))
	{
	    std::string filemaps;
	    if(DialogSelectMapsFileList(lists, filemaps) && filemaps.size()) conf.LoadFileMaps(filemaps);
	    cursor.Hide();
	    levelCursor.Hide();
	    // set first allow color
	    conf.SetMyColor(conf.FirstAllowColor());
	    conf.SetPlayersColors(conf.MyColor());
	    Scenario::RedrawStaticInfo(pointPanel);
	    UpdateCoordOpponentsInfo(pointOpponentInfo, coordColors);
	    Scenario::RedrawOpponentsInfo(pointOpponentInfo);
	    UpdateCoordClassInfo(pointClassInfo, coordClass);
	    Scenario::RedrawClassInfo(pointClassInfo);
	    RedrawRatingInfo(rating);
	    levelCursor.Move(pointDifficultyNormal);
	    levelCursor.Show();
	    cursor.Show();
	    display.Flip();
	}
	else
	// click cancel
	if(le.MouseClickLeft(buttonCancel) || le.KeyPress(KEY_ESCAPE))
	{
	    Settings::Get().SetGameType(Game::UNKNOWN);
	    result = MAINMENU;
	    break;
	}
	else
	// click ok
	if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonOk))
	{
	    if(Settings::Get().Debug()) Error::Verbose("select maps: " + conf.MapsFile());
	    if(Settings::Get().Debug()) Error::Verbose("difficulty: " + Difficulty::String(conf.GameDifficulty()));
	    conf.FixKingdomRandomRace();
	    if(Settings::Get().Debug()) Error::Verbose("select color: " + Color::String(conf.MyColor()));
	    result = STARTGAME;
	    break;
	}
	else
	if(le.MouseClickLeft(box))
	{
	    // select difficulty
	    if(coordDifficulty.end() != (itr = std::find_if(coordDifficulty.begin(), coordDifficulty.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()))))
	    {
		cursor.Hide();
		levelCursor.Move((*itr).x, (*itr).y);
		conf.SetGameDifficulty(Difficulty::Get(itr - coordDifficulty.begin()));
		RedrawRatingInfo(rating);
		cursor.Show();
		display.Flip();
	    }
	    else
	    // select color
	    if(coordColors.end() != (itr = std::find_if(coordColors.begin(), coordColors.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()))))
	    {
		Color::color_t color = Color::GetFromIndex(itr - coordColors.begin());
		if(conf.KingdomColors(color) && conf.AllowColors(color))
		{
		    cursor.Hide();
		    switch(conf.GameType())
		    {
			case Game::NETWORK:
			case Game::HOTSEAT:
			    conf.SetPlayersColors(conf.PlayersColors() & color ? conf.PlayersColors() & ~color : conf.PlayersColors() | color);
			    break;
			default:
			    conf.SetMyColor(color);
			    conf.SetPlayersColors(conf.MyColor());
	    		    break;
		    }
		    Scenario::RedrawOpponentsInfo(pointOpponentInfo);
		    cursor.Show();
		    display.Flip();
		}
	    }
	    else
	    // select class
	    if(coordClass.end() != (itr = std::find_if(coordClass.begin(), coordClass.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()))))
	    {
		Color::color_t color = Color::GetFromIndex(itr - coordClass.begin());
		if(conf.AllowChangeRace(color))
		{
		    cursor.Hide();
		    u8 index = 0;
		    Race::race_t race = conf.KingdomRace(color);
		    switch(race)
		    {
			case Race::KNGT: index = 52; race = Race::BARB; break;
			case Race::BARB: index = 53; race = Race::SORC; break;
			case Race::SORC: index = 54; race = Race::WRLK; break;
			case Race::WRLK: index = 55; race = Race::WZRD; break;
			case Race::WZRD: index = 56; race = Race::NECR; break;
			case Race::NECR: index = 58; race = Race::RAND; break;
			case Race::RAND: index = 51; race = Race::KNGT; break;
			default: break;
		    }
		    conf.SetKingdomRace(color, race);
		    Scenario::RedrawStaticInfo(pointPanel);
		    levelCursor.Redraw();
		    Scenario::RedrawOpponentsInfo(pointOpponentInfo);
		    Scenario::RedrawClassInfo(pointClassInfo);
		    RedrawRatingInfo(rating);
		    cursor.Show();
		    display.Flip();
		}
	    }
	}
	
	if(le.MousePressRight(box))
	{
	    // right info
	    if(le.MousePressRight(buttonSelectMaps)) Dialog::Message(_("Scenario"), _("Click here to select which scenario to play."), Font::BIG);
	    else
	    // difficulty
	    if(coordDifficulty.end() != (itr = std::find_if(coordDifficulty.begin(), coordDifficulty.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()))))
		Dialog::Message(_("Game Difficulty"), _("This lets you change the starting difficulty at which you will play. Higher difficulty levels start you of with fewer resources, and at the higher settings, give extra resources to the computer."), Font::BIG);
	    else
	    // color
	    if(coordColors.end() != (itr = std::find_if(coordColors.begin(), coordColors.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()))) &&
		conf.KingdomColors(Color::GetFromIndex(itr - coordColors.begin())))
		    Dialog::Message(_("Opponents"), _("This lets you change player starting positions and colors. A particular color will always start in a particular location. Some positions may only be played by a computer player or only by a human player."), Font::BIG);
	    else
	    // class
	    if(coordClass.end() != (itr = std::find_if(coordClass.begin(), coordClass.end(), std::bind2nd(RectIncludePoint(), le.MouseCursor()))) &&
		conf.KingdomColors(Color::GetFromIndex(itr - coordClass.begin())))
		    Dialog::Message(_("Class"), _("This lets you change the class of a player. Classes are not always changeable. Depending on the scenario, a player may receive additional towns and/or heroes not of their primary alignment."), Font::BIG);
	    else
	    if(le.MousePressRight(rating.GetRect())) Dialog::Message(_("Difficulty Rating"), _("The difficulty rating reflects a combination of various settings for your game. This number will be applied to your final score."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonOk)) Dialog::Message(_("OK"), _("Click to accept these settings and start a new game."), Font::BIG);
	    else
	    if(le.MousePressRight(buttonCancel)) Dialog::Message(_("Cancel"), _("Click to return to the main menu."), Font::BIG);
	}
    }

    cursor.Hide();

    if(result == STARTGAME)
    {
	if(Settings::Get().Modes(Settings::FADE)) display.Fade();
	// Load maps
	world.LoadMaps(conf.MapsFile());
    }

    return result;
}

u16 GetStepFor(u16 current, u16 width, u16 count)
{
    return current * width * 6 / count + (width * (6 - count) / (2 * count));
}

void UpdateCoordClassInfo(const Point & dst, std::vector<Rect> & rects)
{
    UpdateCoordOpponentsInfo(dst, rects);
}

void UpdateCoordOpponentsInfo(const Point & dst, std::vector<Rect> & rects)
{
    const Settings & conf = Settings::Get();
    const u8 count = conf.KingdomColorsCount();
    const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, 3);
    u8 current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	rects[Color::GetIndex(color)] = conf.KingdomColors(color) ? Rect(dst.x + GetStepFor(current++, sprite.w(), count), dst.y, sprite.w(), sprite.h()) : Rect();
}

void Game::Scenario::RedrawStaticInfo(const Point & pt)
{
    Display & display = Display::Get();
    const Settings & conf = Settings::Get();

    // image panel
    const Sprite &panel = AGG::GetICN(ICN::NGHSBKG, 0);
    display.Blit(panel, pt);

    // text scenario
    Text text(_("Scenario:"), Font::BIG);
    text.Blit(pt.x + (panel.w() - text.w()) / 2, pt.y + 20);

    // maps name
    text.Set(conf.MapsName());
    text.Blit(pt.x + (panel.w() - text.w()) / 2, pt.y + 46);
    
    // text game difficulty
    text.Set(_("Game Difficulty:"));
    text.Blit(pt.x + (panel.w() - text.w()) / 2, pt.y + 75);

    //
    text.Set(_("Easy"), Font::SMALL);
    text.Blit(pt.x + 57 - text.w()/2, 196);
    text.Set(_("Normal"));
    text.Blit(pt.x + 134 - text.w()/2, 196);
    text.Set(_("Hard"));
    text.Blit(pt.x + 210 - text.w()/2, 196);
    text.Set(_("Expert"));
    text.Blit(pt.x + 287 - text.w()/2, 196);
    text.Set(_("Impossible"));
    text.Blit(pt.x + 364 - text.w()/2, 196);

    // text opponents
    text.Set(_("Opponents:"), Font::BIG);
    text.Blit(pt.x + (panel.w() - text.w()) / 2, pt.y + 181);

    // text class
    text.Set(_("Class:"), Font::BIG);
    text.Blit(pt.x + (panel.w() - text.w()) / 2, pt.y + 262);
}

void Game::Scenario::RedrawOpponentsInfo(const Point & dst, const std::vector<Player> *players)
{
    const Settings & conf = Settings::Get();
    const u8 count = conf.KingdomColorsCount();
    u8 current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
    {
	if(conf.KingdomColors(color))
	{
	    u8 index = 0;

	    if(!(conf.AllowColors(color)))
	    {
		switch(color)
		{
		    case Color::BLUE:	index = players ? 39 : 15; break;
		    case Color::GREEN:	index = players ? 40 : 16; break;
		    case Color::RED:	index = players ? 41 : 17; break;
		    case Color::YELLOW:	index = players ? 42 : 18; break;
		    case Color::ORANGE:	index = players ? 43 : 19; break;
		    case Color::PURPLE:	index = players ? 44 : 20; break;
		    default: break;
		}
	    }
	    else
	    if(conf.PlayersColors() & color)
	    {
		switch(color)
		{
		    case Color::BLUE:	index = players ? 33 :  9; break;
		    case Color::GREEN:	index = players ? 34 : 10; break;
		    case Color::RED:	index = players ? 35 : 11; break;
		    case Color::YELLOW:	index = players ? 36 : 12; break;
		    case Color::ORANGE:	index = players ? 37 : 13; break;
		    case Color::PURPLE:	index = players ? 38 : 14; break;
		    default: break;
		}
	    }
	    else
	    {
		switch(color)
		{
		    case Color::BLUE:	index = players ? 27 : 3; break;
		    case Color::GREEN:	index = players ? 28 : 4; break;
		    case Color::RED:	index = players ? 29 : 5; break;
		    case Color::YELLOW:	index = players ? 30 : 6; break;
		    case Color::ORANGE:	index = players ? 31 : 7; break;
		    case Color::PURPLE:	index = players ? 32 : 8; break;
		    default: break;
		}
	    }

	    if(index)
	    {
		const Sprite & sprite = AGG::GetICN(ICN::NGEXTRA, index);
		Display::Get().Blit(sprite, dst.x + GetStepFor(current, sprite.w(), count), dst.y);
		
		// draw name
		if(players)
		{
		    std::vector<Player>::const_iterator itp = std::find_if(players->begin(), players->end(), std::bind2nd(std::mem_fun_ref(&Player::isColor), color));
		    if(players->end() != itp)
		    {
			Text name((*itp).player_name, Font::SMALL);
			name.Blit(dst.x + GetStepFor(current, sprite.w(), count) + (sprite.w() - name.w()) / 2, dst.y + sprite.h() - 14);
		    }
		}

		++current;
	    }
	}
    }
}

void Game::Scenario::RedrawClassInfo(const Point & dst)
{
    Display & display = Display::Get();
    const Settings & conf = Settings::Get();
    const u8 count = conf.KingdomColorsCount();
    u8 current = 0;

    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color)
	if(conf.KingdomColors(color))
    {
	    u8 index = 0;
	    const Race::race_t race = conf.KingdomRace(color);
	    switch(race)
	    {
		case Race::KNGT: index = conf.AllowChangeRace(color) ? 51 : 70; break;
	        case Race::BARB: index = conf.AllowChangeRace(color) ? 52 : 71; break;
	        case Race::SORC: index = conf.AllowChangeRace(color) ? 53 : 72; break;
		case Race::WRLK: index = conf.AllowChangeRace(color) ? 54 : 73; break;
	        case Race::WZRD: index = conf.AllowChangeRace(color) ? 55 : 74; break;
		case Race::NECR: index = conf.AllowChangeRace(color) ? 56 : 75; break;
	        case Race::MULT: index = 76; break;
		case Race::RAND: index = 58; break;
		default: continue;
	    }

    	    const Sprite &sprite = AGG::GetICN(ICN::NGEXTRA, index);
	    display.Blit(sprite, dst.x + GetStepFor(current, sprite.w(), count), dst.y);

	    const std::string & name = (Race::NECR == race ? _("Necroman") : Race::String(race));
	    Text label(name, Font::SMALL);
	    label.Blit(dst.x + GetStepFor(current, sprite.w(), count) + (sprite.w() - label.w()) / 2, dst.y + sprite.h() + 2);

    	    ++current;
    }
}

void RedrawRatingInfo(TextSprite & sprite)
{
    sprite.Hide();
    std::string str(_("Rating %{rating}%"));
    String::Replace(str, "%{rating}", Game::GetRating());
    sprite.SetText(str);
    sprite.Show();
}

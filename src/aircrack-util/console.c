/*
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include <assert.h>
#include <stdio.h>
#include <termios.h>
#ifndef TIOCGWINSZ
#include <sys/termios.h>
#endif
#include <unistd.h>

#include "console.h"

#define channel stdout

void textcolor(int attr, int fg, int bg)
{
	char command[13];

	/* Command is the control command to the terminal */
	snprintf(
		command, sizeof(command), "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void textcolor_fg(int fg)
{
	char command[13];

	/* Command is the control command to the terminal */
	snprintf(command, sizeof(command), "\033[%dm", fg + 30);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void textcolor_bg(int bg)
{
	char command[13];

	/* Command is the control command to the terminal */
	snprintf(command, sizeof(command), "\033[%dm", bg + 40);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void textstyle(int attr)
{
	char command[13];

	/* Command is the control command to the terminal */
	snprintf(command, sizeof(command), "\033[%im", attr);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void reset_term()
{
	struct termios oldt, newt;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void moveto(int x, int y)
{
	char command[13];

	// clamp the X coordinate.
	if (x < 0)
	{
		x = 0;
	}

	// clamp the Y coordinate.
	if (y < 0)
	{
		y = 0;
	}

	// send ANSI sequence to move the cursor.
	snprintf(command, sizeof(command), "%c[%d;%dH", 0x1B, y, x);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void move(int which, int n)
{
	char command[13];
	static const char movement[] = {'A', 'B', 'C', 'D'};

	assert(which >= 0 && which < 4);
	snprintf(command, sizeof(command), "%c[%d%c", 0x1B, n, movement[which]);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void erase_display(int n)
{
	char command[13];

	snprintf(command, sizeof(command), "%c[%dJ", 0x1B, n);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void erase_line(int n)
{
	char command[13];

	snprintf(command, sizeof(command), "%c[%dK", 0x1B, n);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void textcolor_normal(void)
{
	char command[13];

	snprintf(command, sizeof(command), "%c[22m", 0x1B);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void hide_cursor(void)
{
	char command[13];

	snprintf(command, sizeof(command), "%c[?25l", 0x1B);
	fprintf(channel, "%s", command);
	fflush(channel);
}

void show_cursor(void)
{
	char command[13];

	snprintf(command, sizeof(command), "%c[?25h", 0x1B);
	fprintf(channel, "%s", command);
	fflush(channel);
}

int mygetch(void)
{
	struct termios oldt, newt;
	int ch;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}

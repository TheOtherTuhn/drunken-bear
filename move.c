#include "move.h"

int get_to_x_move(move m)
{
    return (m.to & (7<<3))>>3;
}

int get_to_y_move(move m)
{
    return m.to & 7;
}

int get_from_x_move(move m)
{
    return (m.from & (7<<3))>>3;
}

int get_from_y_move(move m)
{
    return m.from & 7;
}

int sprint_move(char *str, move m)
{
    char t = 'X';
    switch(m.type) {
        case Null: t = 'N'; break;
        case Set:  t = 'S'; break;
        case Run:  t = 'R'; break;
        default:   t = 'X'; break;
    }
    return sprintf(str, "%c: (%d %d), (%d %d)", t, get_from_x_move(m), get_from_y_move(m), get_to_x_move(m), get_to_y_move(m));
}

int sprint_move_xml(char *str, move m, char *sid)
{
    int n;
    n = sprintf(str, "<room roomId=\"%s\">\n", sid);
	switch(m.type) {
		case Null: n += sprintf(str+n, "<data class=\"NullMove\"/>\n"); break;
		case Set: n += sprintf(str+n, "<data class=\"SetMove\" setX=\"%d\" setY=\"%d\"/>\n", get_to_x_move(m), get_to_y_move(m)); break;
		case Run: n += sprintf(str+n, "<data class=\"RunMove\" fromX=\"%d\" fromY=\"%d\" toX=\"%d\" toY=\"%d\"/>\n", get_from_x_move(m), get_from_y_move(m),
                          get_to_x_move(m), get_to_y_move(m)); break;
		default: n += sprintf(str+n, "Move not valid! {type:%d, from:%d, to:%d}\n", m.type, m.from, m.to); break;
	}
    n += sprintf(str+n, "</room>\n");
	return n;
}

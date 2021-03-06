struct _draw_tool;
typedef struct _draw_tool DrawTool;

struct _draw_tool {
	gboolean line, fill;
	GdkRGBA *line_col, *fill_col;
	gchar *prop1_label, *prop1_val, *prop2_label, *prop2_val;
	void (*draw) (DrawTool *, cairo_t *, gdouble, gdouble, gdouble, gdouble);
};

typedef struct _draw_tool_circle {
	DrawTool parent;
} DrawToolCircle;

typedef struct _draw_tool_line {
	DrawTool parent;
} DrawToolLineSegment;

typedef struct _draw_app {
	gboolean last_btn_press_valid;
	gdouble last_btn_x, last_btn_y,
	        curr_btn_x, curr_btn_y;
	DrawTool *current_tool;
	GResource *app_res;
	GtkWidget *drawing_area;
	GtkLabel *label1, *label2;
	GtkEntry *prop1, *prop2;
} DrawApp;

void draw_tool_circle_init(DrawToolCircle *circle_tool);
void draw_tool_line_init(DrawToolLineSegment *line_tool);
void draw_app_init(DrawApp *draw_app);

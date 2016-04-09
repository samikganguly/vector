static GdkRGBA tmp1 = {0, 0, 0, 1}, tmp2 = {1, 1, 1, 1};
static void 
draw_circle(DrawTool *this, cairo_t *ctx, gdouble x1, gdouble y1, gdouble x2, gdouble y2) {
	DrawToolCircle *ctool = (DrawToolCircle *)this;
	DrawTool *circle_tool = &(ctool->parent);
	const gdouble rx = abs(x2 - x1), ry = abs(y2 - y1);
	if(rx > 0 || ry > 0 && circle_tool->line || circle_tool->fill) {
		const gdouble radius = sqrt(rx * rx + ry * ry);
		cairo_arc(ctx, x1, y1, radius, 0, M_PI * 2);
		cairo_set_source_rgba(ctx, circle_tool->line_col->red, 
			circle_tool->line_col->green, circle_tool->line_col->blue,
			circle_tool->line_col->alpha);
		cairo_set_line_width(ctx, 1);
		if(circle_tool->fill) {
			cairo_stroke_preserve(ctx);
			cairo_set_source_rgba(ctx, circle_tool->fill_col->red, 
				circle_tool->fill_col->green, circle_tool->fill_col->blue,
				circle_tool->fill_col->alpha);
			cairo_fill(ctx);
		} else
			cairo_stroke(ctx);
	}
}

static void
draw_line(DrawTool *this, cairo_t *ctx, gdouble x1, gdouble y1, gdouble x2, gdouble y2) {
	DrawToolLineSegment *ltool = (DrawToolLineSegment *)this;
	DrawTool *line_tool = &(ltool->parent);
	const gdouble rx = abs(x2 - x1), ry = abs(y2 - y1);
	if(rx > 0 || ry > 0 && line_tool->line) {
		cairo_set_source_rgba(ctx, line_tool->line_col->red, 
			line_tool->line_col->green, line_tool->line_col->blue,
			line_tool->line_col->alpha);
		cairo_set_line_width(ctx, 1);
		cairo_move_to(ctx, x1, y1);
		cairo_line_to(ctx, x2, y2);
		cairo_stroke(ctx);
	}
}

void draw_tool_circle_init(DrawToolCircle *circle_tool) {
	circle_tool->parent.line = TRUE;
	circle_tool->parent.fill = FALSE;
	circle_tool->parent.line_col = &tmp1;
	circle_tool->parent.fill_col = &tmp2;
	circle_tool->parent.prop1_label = "Circle centre";
	circle_tool->parent.prop2_label = "Circle radius";
	circle_tool->parent.draw = draw_circle;
}

void draw_tool_line_init(DrawToolLineSegment *line_tool) {
	line_tool->parent.line = TRUE;
	line_tool->parent.fill = FALSE;
	line_tool->parent.line_col = &tmp1;
	line_tool->parent.fill_col = &tmp2;
	line_tool->parent.prop1_label = "Line head";
	line_tool->parent.prop2_label = "Line tail";
	line_tool->parent.draw = draw_line;
}

void draw_app_init(DrawApp *draw_app) {
	draw_app->last_btn_press_valid = FALSE;
	draw_app->last_btn_x = 0;
	draw_app->last_btn_y = 0;
	draw_app->curr_btn_x = 0;
	draw_app->curr_btn_y = 0;
	draw_app->current_tool = NULL;
	draw_app->app_res = NULL;
	draw_app->drawing_area = NULL;
}

CFLAGS=`pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`

default: draw draw-resource.gresource
draw: draw_types.h draw_types.c draw.c
	$(CC) draw.c -o draw -lm $(CFLAGS) $(LDFLAGS)
draw-resource.gresource: draw-resource.xml
	glib-compile-resources draw-resource.xml

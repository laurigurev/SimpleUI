struct sui_context;
struct sui_widget;
struct sui_color;

void sui_button(struct sui_context* sui, struct sui_widget* widget);

struct sui_color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};

struct sui_widget {
	char* str;
	struct sui_color color;
	struct sui_color hcolor;
	float scale;
	float rotation;
	// float smoothness;
	float margin;
	float padding;
	// ? float animation_speed;
	// previous state
	// current state
	// overlapped areas
};

void sui_button(struct sui_context* sui, struct sui_widget* widget)
{
	// transform to a fitting scale and rotation
	// check against mouse coordinates
}

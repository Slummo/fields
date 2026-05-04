#define KS_RENDER_IMPL

#include <ks/core.h>
#include <ks/time.h>
#include <ks/math.h>
#include <cx/render.h>

#define WIN_WIDTH 960
#define WIN_HEIGHT 540
#define GRID_WIDTH 50
#define GRID_HEIGHT 50
#define GRID_CELLSIZE 0.1
#define WAVE_FREQ 0.5
#define WAVE_SPEED 5.0

static cx_shader g_rect_shader = {0};
static cx_shader g_triangle_shader = {0};
static ks_mat4 g_proj = {0};

static void set_projections(int32_t width, int32_t height) {
    float aspect = (float)width / height;
    float physw = GRID_WIDTH * GRID_CELLSIZE;
    float physh = GRID_HEIGHT * GRID_CELLSIZE;
    float projw = physw;
    float projh = physh;
    if (aspect > 1.0f) {
        projw *= aspect;
    } else {
        projh /= aspect;
    }

    // Use projw and projh to respect proportions
    ks_mat4_ortho(&g_proj, 0.0f, physw, 0.0f, physh, 0.1f, 100.0f);

    cx_shader_bind(g_rect_shader);
    cx_shader_set_mat4(g_rect_shader, "uProj", ks_mat4_ptr(&g_proj));
    cx_shader_bind(g_triangle_shader);
    cx_shader_set_mat4(g_triangle_shader, "uProj", ks_mat4_ptr(&g_proj));
}

static void wave(ks_field_ctx* ctx, void* out) {
    float* o = out;
    ks_vec2 p = KS_VEC2(ctx->x, ctx->y);
    ks_vec2 c = KS_VEC2(ctx->dst->width / 2.0f, ctx->dst->height / 2.0f);
    float dist = ks_vec2_dist(&p, &c);
    float ripple = sinf(dist * WAVE_FREQ - ctx->t * WAVE_SPEED);
    float falloff = 1.0f / (1.0f * dist * 0.1f);
    *o = ripple * falloff;
}

static void gradient(ks_field_ctx* ctx, void* out) {
    ks_vec2* o = out;
    *o = ks_sf2_grad(ctx->src, ctx->x, ctx->y);
}

static void divergence(ks_field_ctx* ctx, void* out) {
    float* o = out;
    *o = ks_vf2_div(ctx->src, ctx->x, ctx->y);
}

KS_UNUSED static void curl(ks_field_ctx* ctx, void* out) {
    float* o = out;
    *o = ks_vf2_curl(ctx->src, ctx->x, ctx->y);
}

static void lap(ks_field_ctx* ctx, void* out) {
    float* o = out;
    *o = ks_sf2_lap(ctx->src, ctx->x, ctx->y);
}

int main(void) {
    cx_render_init(WIN_WIDTH, WIN_HEIGHT, "Fields");
    cx_win_resize_cb(set_projections);
    ks_time_init(60.0f);

    const ks_field_cb scalar_sample = wave;
    const ks_field_cb grad_sample = gradient;
    const ks_field_cb div_sample = divergence;
    const ks_field_cb lap_sample = lap;

    ks_field scalar_field = ks_field_create(GRID_WIDTH, GRID_HEIGHT, 1, sizeof(float), GRID_CELLSIZE);
    ks_field grad_field = ks_field_create(GRID_WIDTH, GRID_HEIGHT, 1, sizeof(ks_vec2), GRID_CELLSIZE);
    ks_field div_field = ks_field_create(GRID_WIDTH, GRID_HEIGHT, 1, sizeof(float), GRID_CELLSIZE);
    ks_field lap_field = ks_field_create(GRID_WIDTH, GRID_HEIGHT, 1, sizeof(float), GRID_CELLSIZE);

    // clang-format off
    const float rect_verts[] = {
         0.0f,  0.0f,
         1.0f,  0.0f,
         1.0f,  1.0f, 
         0.0f,  1.0f 
    };

    const float triangle_verts[] = {
        0.0f, -0.25f,
        0.0f,  0.25f,
        1.0f,  0.0f,
    };

    const uint8_t rect_inds[] = {0, 1, 2, 0, 2, 3};
    // clang-format on

    cx_mesh rect = cx_mesh_create();
    cx_mesh_load_vertices(&rect, 4, rect_verts, &CX_VFMT_POS2);
    cx_mesh_load_indices(&rect, 6, rect_inds, &KS_IFMT_U8);
    cx_mesh_load_instances(&rect, scalar_field.width * scalar_field.height, scalar_field.data, &CX_VFMT_INST_FLOAT);
    g_rect_shader = cx_shader_create("assets/scalar.vert", "assets/scalar.frag");

    cx_mesh triangle = cx_mesh_create();
    cx_mesh_load_vertices(&triangle, 3, triangle_verts, &CX_VFMT_POS2);
    cx_mesh_load_instances(&triangle, grad_field.width * grad_field.height, grad_field.data, &CX_VFMT_INST_VEC2);
    g_triangle_shader = cx_shader_create("assets/vector.vert", "assets/vector.frag");

    float min_val = -1.0f;
    float max_val = 1.0f;

    set_projections(WIN_WIDTH, WIN_HEIGHT);

    ks_mat4 rect_view;
    ks_mat4_lookat(&rect_view, &KS_VEC3(0.0f, 0.0f, 3.0f), &KS_VEC3(0.0f, 0.0f, 0.0f), &KS_VEC3(0.0f, 1.0f, 0.0f));
    cx_shader_bind(g_rect_shader);
    cx_shader_set_int(g_rect_shader, "uGridWidth", &scalar_field.width);
    cx_shader_set_float(g_rect_shader, "uCellSize", &scalar_field.cellsize);
    cx_shader_set_mat4(g_rect_shader, "uView", ks_mat4_ptr(&rect_view));
    cx_shader_set_mat4(g_rect_shader, "uProj", ks_mat4_ptr(&g_proj));
    cx_shader_set_float(g_rect_shader, "uMinVal", &min_val);
    cx_shader_set_float(g_rect_shader, "uMaxVal", &max_val);

    ks_mat4 triangle_view;
    ks_mat4_lookat(&triangle_view, &KS_VEC3(0.0f, 0.0f, 3.0f), &KS_VEC3(0.0f, 0.0f, 0.0f), &KS_VEC3(0.0f, 1.0f, 0.0f));
    cx_shader_bind(g_triangle_shader);
    cx_shader_set_int(g_triangle_shader, "uGridWidth", &grad_field.width);
    cx_shader_set_float(g_triangle_shader, "uCellSize", &grad_field.cellsize);
    cx_shader_set_mat4(g_triangle_shader, "uView", ks_mat4_ptr(&triangle_view));
    cx_shader_set_mat4(g_triangle_shader, "uProj", ks_mat4_ptr(&g_proj));

    float time = 0.0f;

    while (!cx_win_should_close()) {
        ks_time_update();

        while (ks_time_consume_fixed()) {
            float fixed_dt = ks_time_fixed_dt();
            time += fixed_dt;
            ks_field_sample(&scalar_field, NULL, time, scalar_sample);
            ks_field_sample(&grad_field, &scalar_field, time, grad_sample);
            ks_field_sample(&div_field, &grad_field, time, div_sample);
            ks_field_sample(&lap_field, &scalar_field, time, lap_sample);
        }

        ks_vec2 size = cx_win_size();
        int32_t fw = size.x / 2;
        int32_t fh = size.y / 2;

        cx_begin_drawing();
        cx_background(CX_COL4(0.1f, 0.1f, 0.1f, 1.0f));

        // Wave scalar field
        cx_drawbox(0, fh, fw, fh);
        cx_mesh_update_instances(&rect, scalar_field.width * scalar_field.height, scalar_field.data,
                                 &CX_VFMT_INST_FLOAT);
        cx_mesh_draw(&rect, g_rect_shader);

        // Gradient vector field
        cx_drawbox(fw, fh, fw, fh);
        cx_mesh_update_instances(&triangle, grad_field.width * grad_field.height, grad_field.data, &CX_VFMT_INST_VEC2);
        cx_mesh_draw(&triangle, g_triangle_shader);

        // Divergence scalar field
        cx_drawbox(0, 0, fw, fh);
        cx_mesh_update_instances(&rect, div_field.width * div_field.height, div_field.data, &CX_VFMT_INST_FLOAT);
        cx_mesh_draw(&rect, g_rect_shader);

        // Laplacian scalar field
        cx_drawbox(fw, 0, fw, fh);
        cx_mesh_update_instances(&rect, lap_field.width * lap_field.height, lap_field.data, &CX_VFMT_INST_FLOAT);
        cx_mesh_draw(&rect, g_rect_shader);

        cx_drawbox_reset();
        cx_end_drawing();
    }

    cx_mesh_destroy(&rect);
    cx_mesh_destroy(&triangle);
    ks_field_destroy(&scalar_field);
    ks_field_destroy(&lap_field);
    ks_field_destroy(&div_field);
    ks_field_destroy(&grad_field);

    cx_render_terminate();

    return EXIT_SUCCESS;
}
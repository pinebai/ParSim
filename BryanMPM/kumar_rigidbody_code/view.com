gfx read data 0.exdata time  0.0000;
gfx read data 1.exdata time  0.0500;
gfx read data 2.exdata time  0.1000;
gfx read data 3.exdata time  0.1500;
gfx read data 4.exdata time  0.2000;
gfx read data 5.exdata time  0.2500;
gfx read data 6.exdata time  0.3000;
gfx read data 7.exdata time  0.3500;
gfx read data 8.exdata time  0.4000;
gfx read data 9.exdata time  0.4500;
gfx read data 10.exdata time  0.5000;
gfx read data 11.exdata time  0.5500;
gfx read data 12.exdata time  0.6000;
gfx read data 13.exdata time  0.6500;
gfx read data 14.exdata time  0.7000;
gfx read data 15.exdata time  0.7500;
gfx read data 16.exdata time  0.8000;
gfx read data 17.exdata time  0.8500;
gfx read data 18.exdata time  0.9000;
gfx read data 19.exdata time  0.9500;
gfx read data 20.exdata time  1.0000;

gfx create axes length 500;
gfx draw axes;

gfx modify spectrum default linear reverse range   3.6068   6.1644 extend_above extend_below rainbow colour_range 0 1 component 1;

gfx modify g_element projectile general clear circle_discretization 6 default_coordinate coordinates element_discretization "4*4*4" native_discretization none;
gfx modify g_element projectile data_points glyph sphere general size "50*50*50" centre 0,0,0 font default select_on material bone selected_material default_selected;
gfx modify g_element projectile data_points glyph arrow_solid general size "20*20*20" centre 0,0,0 font default orientation velocity variable_scale velocity_magnitude scale_factors "1*1*1" select_on material bone data velocity_magnitude spectrum default selected_material default_selected;
gfx modify g_element projectile data_points glyph arrow_line general size "100*100*100" centre 0,0,0 font default orientation principal_axis1 scale_factors "1*1*1" select_on material bone selected_material default_selected;
gfx modify g_element projectile data_points glyph arrow_line general size "100*100*100" centre 0,0,0 font default orientation principal_axis2 scale_factors "1*1*1" select_on material bone selected_material default_selected;
gfx modify g_element projectile data_points glyph arrow_line general size "100*100*100" centre 0,0,0 font default orientation principal_axis3 scale_factors "1*1*1" select_on material bone selected_material default_selected;
gfx create window 1 double_buffer;
gfx modify window 1 image scene default light_model default;
gfx modify window 1 image add_light default;
gfx modify window 1 layout simple ortho_axes z -y eye_spacing 0.25 width 1083 height 643;
gfx modify window 1 set current_pane 1;
gfx modify window 1 background colour 0 0 0 texture none;
gfx modify window 1 view parallel eye_point 3020.05 -1370.6 1309.04 interest_point 1537.73 1228.84 702.155 up_vector 0.0176225 0.236839 0.971389 view_angle 62.3129 near_clipping_plane 30.533 far_clipping_plane 10911.5 relative_viewport ndc_placement -1 1 2 2 viewport_coordinates 0 0 1 1;
gfx modify window 1 overlay scene none;
gfx modify window 1 set transform_tool current_pane 1 std_view_angle 40 normal_lines no_antialias depth_of_field 0.0 fast_transparency blend_normal;

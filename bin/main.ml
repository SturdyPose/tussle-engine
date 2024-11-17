open Tussle_engine.Drawing
(* open Tgl4 *)

let () =
  (* Initialize the library *)
  GLFW.init ();
  at_exit GLFW.terminate;
  (* Set gl version *)
  GLFW.windowHint ~hint:GLFW.ContextVersionMajor ~value:4;
  GLFW.windowHint ~hint:GLFW.ContextVersionMinor ~value:6;
  (* Create a windowed mode window and its OpenGL context *)
  let window = GLFW.createWindow ~width:640 ~height:480 ~title:"Tussle on DEEZ" () in
  (* Make the window's context current *)
  GLFW.makeContextCurrent ~window:(Some window);
  if( glew_init () |> not) then print_endline "Failed to initialize GLEW" else ();
  (* Loop until the user closes the window *)
  let vertex_shader_source = "#version 330 core
  layout (location = 0) in vec3 aPos;

  void main()
  {
      gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  }\x00" in
  let vertex_shader = gl_create_shader GL_VERTEX_SHADER in
  gl_shader_source vertex_shader 1 vertex_shader_source None |> ignore;
  gl_compile_shader vertex_shader |> ignore;

  let fragment_shader_source = "#version 330 core
  out vec4 FragColor;

  void main()
  {
      FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }\x00" in
  let fragment_shader = gl_create_shader GL_FRAGMENT_SHADER in
  gl_shader_source fragment_shader 1 fragment_shader_source None |> ignore;
  gl_compile_shader fragment_shader |> ignore;

  let shader_program = gl_create_program () in
  gl_attach_shader shader_program vertex_shader |> ignore;
  gl_attach_shader shader_program fragment_shader |> ignore;
  gl_link_program shader_program |> ignore;
  gl_use_program shader_program |> ignore;

  let vertices = [| 
    -0.5; 0.1; 0.0; 
    -0.5; 0.5; 0.0;
    0.5; 0.1; 0.0; 
    0.5; 0.5; 0.0;

    -0.5; -0.5; 0.0; 
    -0.5; -0.1; 0.0;
    0.5; -0.5; 0.0; 
    0.5; -0.1; 0.0;

    |] in

  let element_buffer = [|
    0;1;2;
    1;2;3;

    4;5;6;
    5;6;7;
    |] in
  let vbo = gl_gen_buffer () in
  let vao = gl_gen_vertex_array () in
  gl_bind_vertex_array vao |> ignore;
  gl_bind_buffer GL_ARRAY_BUFFER vbo |> ignore;
  gl_buffer_data GL_ARRAY_BUFFER ((Array.length vertices)) (FloatData vertices) GL_STATIC_DRAW |> ignore;
  gl_vertex_attrib_pointer 0 3 GL_FLOAT false (3 * 4) |> ignore;
  gl_enable_vertex_attrib_array 0 |> ignore;

  while not (GLFW.windowShouldClose ~window:window) do
    (* Render here *)
    gl_clear_color ~r:0.0 ~g:0.0 ~b:1.0 ~a:0.0 |> ignore;
    gl_clear [GL_COLOR_BUFFER_BIT; GL_DEPTH_BUFFER_BIT] |> ignore;
    (* Gl.clear_color 255.0 0.0 0.0 0.0; *)
    (* Swap front and back buffers *)
    (* Poll for and process events *)

    (* draw (); *)
    gl_use_program shader_program |> ignore;
    (* gl_draw_arrays GL_TRIANGLES 0 6 |> ignore; *)

    gl_draw_elements GL_TRIANGLES (Array.length element_buffer) GL_UNSIGNED_INT element_buffer |> ignore;

    GLFW.swapBuffers ~window:window;
    GLFW.pollEvents ();
  done

  

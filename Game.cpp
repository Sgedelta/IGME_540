#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "SimpleShader.h"
#include "Material.h"
#include <memory>
#include <iostream>
#include <format>

//include ImGui files
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateShaderToEntity();
	CreateCameras();

	


	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	}

	//Initialize ImGui and Platform/Renderer Backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());

	//ImGui dark mode:
	ImGui::StyleColorsDark();


	//ImGui inital data:
	ImGui_bgColor[0] = 0.4f;
	ImGui_bgColor[1] = 0.6f;
	ImGui_bgColor[2] = 0.75f;
	ImGui_bgColor[3] = 0.0f;

	ImGui_offset[0] = 0.0f;
	ImGui_offset[1] = 0.0f;
	ImGui_offset[2] = 0.0f;

	ImGui_colorTint[0] = 1.0f;
	ImGui_colorTint[1] = 1.0f;
	ImGui_colorTint[2] = 1.0f;
	ImGui_colorTint[3] = 1.0f;
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	//ImGui Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


void Game::CreateCameras()
{
	cameraPtrs.push_back(std::make_shared<Camera>(Window::AspectRatio(), XMFLOAT3(0, 2, -5)));
	cameraPtrs.push_back(std::make_shared<Camera>(Window::AspectRatio(), XMFLOAT3(0, -2, 5)));
	cameraPtrs.push_back(std::make_shared<Camera>(Window::AspectRatio(), XMFLOAT3(-5, 0, -5)));
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateShaderToEntity()
{
	//Old Code:
	{
		/*
		* OLD CODE:
		// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 magenta = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 cyan = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex triVerts[] =
	{
		{ XMFLOAT3(-0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), green },
	};

	int triIndexes[] = { 0, 2, 1 };

	meshPtrs.push_back(std::make_shared<Mesh>(triVerts, 3, triIndexes, 3));

	//make a square
	Vertex squareVerts[] = {
		{ XMFLOAT3(+0.125f, +0.125f, +0.0f), black},
		{ XMFLOAT3(+0.125f, -0.125f, +0.0f), white},
		{ XMFLOAT3(-0.125f, -0.125f, +0.0f), black},
		{ XMFLOAT3(-0.125f, +0.125f, +0.0f), white}
	};

	int squareIndexes[] = { 0, 1, 2, 0, 2, 3 };

	meshPtrs.push_back(std::make_shared<Mesh>(squareVerts, 4, squareIndexes, 6));

	//make amogus
	Vertex amongVerts[] = {
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), magenta},
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), cyan},
		{ XMFLOAT3(+0.3f, +0.5f, +0.0f), yellow},
		{ XMFLOAT3(+0.3f, +0.3f, +0.0f), magenta},
		{ XMFLOAT3(+0.33f, +0.3f, +0.0f), cyan},
		{ XMFLOAT3(+0.33f, +0.2f, +0.0f), yellow},
		{ XMFLOAT3(+0.3f, +0.2f, +0.0f), magenta},
		{ XMFLOAT3(+0.3f, +0.0f, +0.0f), cyan},
		{ XMFLOAT3(+0.19f, +0.0f, +0.0f), yellow},
		{ XMFLOAT3(+0.19f, +0.2f, +0.0f), magenta},
		{ XMFLOAT3(+0.11f, +0.2f, +0.0f), cyan},
		{ XMFLOAT3(+0.11f, +0.0f, +0.0f), yellow}
	};

	int amongIndexes[] = {
		0, 10, 11,
		0, 1, 10,
		1, 2, 10,
		10, 2, 9,
		2, 3, 9,
		3, 4, 5,
		3, 5, 6,
		9, 3, 6,
		9, 6, 7,
		9, 7, 8
	};
	
	meshPtrs.push_back(std::make_shared<Mesh>(amongVerts, 12, amongIndexes, 30));
		*/
	}


	//load shaders:
	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> uvDebugPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugUVsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> normalDebugPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugNormalsPS.cso").c_str());

	//make materials:
	std::vector<std::shared_ptr<Material>> materials;

	materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), vs, ps)); //tintMaterial
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0, 0, 0, 1), vs, uvDebugPS)); //uvDebugMaterial
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0, 0, 0, 1), vs, normalDebugPS)); //normalDebugMaterial

	//load meshes
	meshPtrs.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str()));
	meshPtrs.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str()));
	meshPtrs.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str()));
	meshPtrs.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Models/torus.obj").c_str()));


	//make entities
		//Note: when we make an entity, make sure we're adding the float arrays to entityData
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < meshPtrs.size(); j++) {
			entityPtrs.push_back(std::make_shared<Entity>(meshPtrs[j], materials[i]));
			
			//position:
			entityData.push_back(j * 2.5); //x
			entityData.push_back(i * 3); //y
			entityData.push_back(0); //z

			//rotation:
			entityData.push_back(0);
			entityData.push_back(0);
			entityData.push_back(0);

			//scale:
			entityData.push_back(1);
			entityData.push_back(1);
			entityData.push_back(1);

			//set position and stuff:
			entityPtrs[meshPtrs.size() * i + j].get()->GetTransform()->SetPosition(j * 2.5, i*3, 0);
			
		}
	}
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (int i = 0; i < cameraPtrs.size(); ++i) {
		cameraPtrs[i].get()->UpdateProjectionMatrix(Window::AspectRatio());
	}
	
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	//Update ImGui information. This MUST run first.
	UpdateImGui(deltaTime);

	//Build the Debug UI
	BuildUI();


	//update Camera:
	if (cameraIndex < cameraPtrs.size()) {
		cameraPtrs[cameraIndex].get()->Update(deltaTime);
	}

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	ImGui_bgColor);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	/*
	{
		// Set buffers in the input assembler (IA) stage
		//  - Do this ONCE PER OBJECT, since each object may have different geometry
		//  - For this demo, this step *could* simply be done once during Init()
		//  - However, this needs to be done between EACH DrawIndexed() call
		//     when drawing different geometry, so it's here as an example
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Tell Direct3D to draw
		//  - Begins the rendering pipeline on the GPU
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all currently set Direct3D resources (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		Graphics::Context->DrawIndexed(
			3,     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}
	*/

	//Draw entities
	for (int i = 0; i < entityPtrs.size(); ++i) {
		if (cameraIndex < cameraPtrs.size()) {
			entityPtrs[i].get()->Draw(ImGui_colorTint, cameraPtrs[cameraIndex].get());
		}
	}

	//Last thing to draw: ImGui!
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}


//ImGui Helper Function
void Game::UpdateImGui(float deltaTime) {
	// Feed Fresh Data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();

	// Reset frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// show demo window
	//ImGui::ShowDemoWindow();
}

//Helper Function that builds our ImGui UI
void Game::BuildUI() {
	//start a new window
	ImGui::Begin("A Cool New Window");

	ImGui::Text("Current Framerate: %d", ImGui::GetIO().Framerate);
	ImGui::Text("Current Window Dimensions: %d, %d", Window::Width(), Window::Height());

	ImGui::ColorEdit4("Background Color", ImGui_bgColor);

	if (ImGui::Button("Toggle ImGui Demo Window")) {
		ImGui_Demo_Show = !ImGui_Demo_Show;
	}

	if (ImGui_Demo_Show) {
		ImGui::ShowDemoWindow();
	}

	if (ImGui::CollapsingHeader("Collapsable Header")) {

		static int highlightedIndex = -1;
		const char* items[] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten!!!!"};
		if (ImGui::BeginListBox("Cool Listbox!")) {
			for (int i = 0; i < 10; ++i) {
				const bool isSelected = (highlightedIndex == i);
				if (ImGui::Selectable(items[i], isSelected)) {
					highlightedIndex = i;
				}
				
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}
	}

	static float vals[90] = {};
	static int valOffset = 0;
	static double refreshTime = 0.0;
	if (refreshTime == 0)
		refreshTime = ImGui::GetTime();

	while (refreshTime < ImGui::GetTime()) 
	{
		static float phase = 0.0f;
		vals[valOffset] = (float)sin(phase * refreshTime);
		valOffset = (valOffset + 1) % 90;
		phase += .1f * valOffset;
		refreshTime += 1.0f / 60.0f;
	}

	ImGui::PlotLines("A Really Cool Plot!!!!!", vals, 90, valOffset);



	ImGui::End(); //end A Cool New Window

	//Mesh Info
	ImGui::Begin("Mesh Information");
	ImGui::SeparatorText("Tint Scene: ");
	ImGui::DragFloat4("color tint", &ImGui_colorTint[0], 0.01f, 0.0f, 1.0f);

	ImGui::SeparatorText("Meshes and Entities:");

	if (ImGui::CollapsingHeader("Mesh Information")) {
		for (int i = 0; i < meshPtrs.size(); ++i) {
			if (ImGui::CollapsingHeader(std::format("Mesh {}", i).c_str())) {
				ImGui::Text("Verts: %d", meshPtrs[i].get()->GetVertextCount());
				ImGui::Text("Indices: %d", meshPtrs[i].get()->GetIndexCount());
				ImGui::Text("Tris: %d", meshPtrs[i].get()->GetIndexCount()/3);
			}
		}
	}

	if (ImGui::CollapsingHeader("Entity Information")) {
		for (int i = 0; i < entityPtrs.size(); ++i) {
			if (ImGui::CollapsingHeader(std::format("Entity {}", i).c_str())) {

				float pos[3] = { entityData[i * 9], entityData[i * 9 + 1], entityData[i * 9 + 2] };
				float rot[3] = { entityData[i * 9 + 3], entityData[i * 9 + 4], entityData[i * 9 + 5] };
				float scale[3] = { entityData[i * 9 + 6], entityData[i * 9 + 7], entityData[i * 9 + 8] };

				ImGui::DragFloat3(std::format("Position {}", i).c_str(), pos, .01f, -1.0f, 1.0f);
				ImGui::DragFloat3(std::format("Rotation {}", i).c_str(), rot, .01f, -2.0f * 3.14159265358979f, 2.0f * 3.14159265358979f);
				ImGui::DragFloat3(std::format("Scale {}", i).c_str(), scale, .01f, -1000.0f, 1000.0f);

				//I hate this....
				entityData[i * 9] = pos[0];
				entityData[i * 9 + 1] = pos[1];
				entityData[i * 9 + 2] = pos[2];
				entityData[i * 9 + 3] = rot[0];
				entityData[i * 9 + 4] = rot[1];
				entityData[i * 9 + 5] = rot[2];
				entityData[i * 9 + 6] = scale[0];
				entityData[i * 9 + 7] = scale[1];
				entityData[i * 9 + 8] = scale[2];

				entityPtrs[i].get()->GetTransform().get()->SetPosition(pos[0], pos[1], pos[2]);
				entityPtrs[i].get()->GetTransform().get()->SetRotation(rot[0], rot[1], rot[2]);
				entityPtrs[i].get()->GetTransform().get()->SetScale(scale[0], scale[1], scale[2]);

			}
		}
	}

	ImGui::End(); //end Mesh Information

	ImGui::Begin("Cameras");
		ImGui::SeparatorText(std::format("Camera Index Is: {}", cameraIndex).c_str());
		if (ImGui::Button("Next Camera")) {
			cameraIndex += 1;
			cameraIndex %= (int)cameraPtrs.size();
			printf("%d", cameraIndex);
		}
		if (ImGui::Button("Prev Camera")) {
			cameraIndex += (int)cameraPtrs.size()-1;
			cameraIndex %= cameraPtrs.size();
			printf("%d", cameraIndex);
		}
	ImGui::End();

}



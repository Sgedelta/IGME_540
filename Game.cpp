#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Mesh.h"
#include "BufferStructs.h"
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
	LoadShaders();
	CreateGeometry();
	CreateBuffers();
	CreateEntities();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
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


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}

//a helper method that initializes many buffers for us, such as const buffers for shaders
void Game::CreateBuffers() {

	//Vertex Constant Buffer:
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexBufferDesc.ByteWidth = (1 * (sizeof(VertexShaderData) + 15)) / 16 * 16; //integer division trick to normalize to a value of 16
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//create:
	Graphics::Device->CreateBuffer(&vertexBufferDesc, 0, vertexConstBuffer.GetAddressOf());
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
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
}

void Game::CreateEntities()
{
	//Note: when we make an entity, make sure we're adding the float arrays to entityData
	for (int i = 0; i < meshPtrs.size(); i++) {
		entityPtrs.push_back(std::make_shared<Entity>(meshPtrs[i]));
		for (int dataCount = 0; dataCount < 6; ++dataCount) {
			entityData.push_back(0);
		}
		for (int dataCount = 0; dataCount < 3; ++dataCount) {
			entityData.push_back(1);
		}
	}
	
	entityPtrs.push_back(std::make_shared<Entity>(meshPtrs[2]));
	entityPtrs.push_back(std::make_shared<Entity>(meshPtrs[2]));
	entityPtrs.push_back(std::make_shared<Entity>(meshPtrs[2]));
	for (int entityCount = 0; entityCount < 3; ++entityCount) {
		for (int dataCount = 0; dataCount < 6; ++dataCount) {
			entityData.push_back(0);
		}
		for (int dataCount = 0; dataCount < 3; ++dataCount) {
			entityData.push_back(1);
		}
	}
	


}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
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

	//send data to GPU
	SendGPUData();

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
		entityPtrs[i].get()->Draw(vertexConstBuffer.Get(), ImGui_colorTint);
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


void Game::SendGPUData() 
{
	
	//Vertex Shader Data:
	VertexShaderData vsData;
	vsData.tint = XMFLOAT4(ImGui_colorTint[0], ImGui_colorTint[1], ImGui_colorTint[2], ImGui_colorTint[3]);
	//vsData.offset = XMFLOAT3(ImGui_offset[0], ImGui_offset[1], ImGui_offset[2]); //TODO: UPDATE

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(vertexConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer); //overwrite and "freeze" GPU

	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData)); // populate actual data

	Graphics::Context->Unmap(vertexConstBuffer.Get(), 0); // "unfreeze" GPU, allowing it read access again

	//bind to vertex Shader
	Graphics::Context->VSSetConstantBuffers(0, 1, vertexConstBuffer.GetAddressOf());
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

}



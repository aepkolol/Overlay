#include "Functions.h"
#include "Offsets.h"

IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;

ID3DXLine* p_Line;
ID3DXFont* pFontSmall = 0;

bool btDebug;
bool hoarders;
bool merchants;
bool orderSouls;

int DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(1);

    ZeroMemory(&p_Params, sizeof(p_Params));
    p_Params.Windowed = TRUE;
    p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    p_Params.BackBufferFormat = D3DFMT_A8R8G8B8 ;
    p_Params.BackBufferWidth = Width;
    p_Params.BackBufferHeight = Height;
    p_Params.EnableAutoDepthStencil = TRUE;
    p_Params.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
		exit(1);

	if (!p_Line)
		D3DXCreateLine(p_Device, &p_Line);

	D3DXCreateFont(p_Device, 12, 0, 400, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &pFontSmall);

	return 0;
}

 Vector2 me = Vector2(180, 180);
 Vector2 actor = Vector2(100, 220);
 float myangle = 180;

int Render()
{
	p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	p_Device->BeginScene();

	if (tWnd == GetForegroundWindow())
	{
		ReadData();
			auto World = mem.Read<ULONG_PTR>(WorldAddress);
			GNames = mem.Read<ULONG_PTR>(NamesAddress);
			ULONG_PTR owninggameinstance = mem.Read<ULONG_PTR>(World + Offsets::OwningGameInstance);
			ULONG_PTR LocalPlayers = mem.Read<ULONG_PTR>(owninggameinstance + Offsets::LocalPlayers);
			ULONG_PTR ULocalPlayer = mem.Read<ULONG_PTR>(LocalPlayers);
			ULONG_PTR PlayerController = mem.Read<ULONG_PTR>(ULocalPlayer + Offsets::PlayerController);
			auto LocalPlayer = mem.Read<ULONG_PTR>(PlayerController + Offsets::Pawn);
			auto LocalPlayeState = mem.Read<ULONG_PTR>(PlayerController + Offsets::PlayerState);
			auto CameraManager = mem.Read<ULONG_PTR>(PlayerController + Offsets::PlayerCameraManager);
			auto RootComponent = mem.Read<ULONG_PTR>(LocalPlayer + Offsets::RootComponent);

			//auto HealthComponent = mem.Read<ULONG_PTR>(LocalPlayer + Offsets::HealthComponent);
			//auto WieldedItemComponent = mem.Read<ULONG_PTR>(LocalPlayer + Offsets::WieldedItemComponent);
			//auto CurrentWieldedItem = mem.Read<ULONG_PTR>(WieldedItemComponent + Offsets::CurrentlyWieldedItem);
			//auto pWieldedItem = mem.Read<ULONG_PTR>(CurrentWieldedItem + Offsets::WieldableItemName);
			//std::wstring ItemWieleded = mem.Read<textx>(pWieldedItem).word;

			ULONG_PTR ULevel = mem.Read<ULONG_PTR>(World + Offsets::PersistentLevel);
			int ActorCount = mem.Read<int>(ULevel + Offsets::ActorsTArrayCount);
			std::vector<Vector3> new_XMarksTheSpot;
			auto LocalNamePointer = mem.Read<ULONG_PTR>(LocalPlayeState + Offsets::PlayerName);
			auto LocalName = mem.Read<textx>(LocalNamePointer);
			std::wstring mename = LocalName.word;
			using convert_type = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_type, wchar_t> converter;
			std::string menamestring = converter.to_bytes(mename);

			for (int i = 0; i < ActorCount; i++)
			{
				ULONG_PTR ActorList = mem.Read<ULONG_PTR>(ULevel + Offsets::ActorsTArray);
				ULONG_PTR Actor = mem.Read<ULONG_PTR>(ActorList + (i * 0x8));

				if (!Actor)
					continue;

				int ActorID = mem.Read<int>(Actor + Offsets::Id);
				auto ActorRootComponet = mem.Read<ULONG_PTR>(Actor + Offsets::RootComponent);
				auto Actorrelativelocation = mem.Read<Vector3>(ActorRootComponet + Offsets::RelativeLocation); // owninggameinstance.LocalPlayersPTR->LocalPlayers->PlayerController->PlayerState->RootComponent->RelativeLocation_0;
				auto ActorYaw = mem.Read<float>(ActorRootComponet + Offsets::RelativeRotationYaw); // owninggameinstance.LocalPlayersPTR->LocalPlayers->PlayerController->PlayerState->RootComponent->RelativeLocation_0;
				std::string name = GetActorName(ActorID);

				if (name.find("IslandService") != std::string::npos)
				{
					IslandDataAsset_PTR = mem.Read<ULONG_PTR>(Actor + Offsets::IslandDataAsset);
				}

				if (name.find("BP_") == std::string::npos)
					continue;

				auto ActorWieldedItemComponent = mem.Read<ULONG_PTR>(Actor + Offsets::WieldedItemComponent);
				auto ActorCurrentWieldedItem = mem.Read<ULONG_PTR>(ActorWieldedItemComponent + Offsets::CurrentlyWieldedItem);
				auto ActorpWieldedItem = mem.Read<ULONG_PTR>(ActorCurrentWieldedItem + Offsets::WieldableItemName);
				std::wstring ActorItemWieleded = mem.Read<textx>(ActorpWieldedItem).word;
				using convert_type = std::codecvt_utf8<wchar_t>;
				std::wstring_convert<convert_type, wchar_t> converter;
				std::string ActorItemWieleded_str = converter.to_bytes(ActorItemWieleded);

				AActors info;
				info.namesize = GetTextWidth(name.c_str(), pFontSmall);
				info.item = ActorItemWieleded_str;

				if (name.find("BP_PlayerPirate_C") != std::string::npos)
				{
					const auto Actorhealthcomponet = mem.Read<ULONG_PTR>(Actor + Offsets::HealthComponent);
					const float Actorhealth = mem.Read<float>(Actorhealthcomponet + Offsets::CurrentHealth);
					const float Actormaxhealth = mem.Read<float>(Actorhealthcomponet + Offsets::MaxHealth);
					const auto ActorPlayerstate = mem.Read<ULONG_PTR>(Actor + Offsets::PlayerState);
					const auto ActorNamePointer = mem.Read<ULONG_PTR>(ActorPlayerstate + Offsets::PlayerName);
					const auto ActorName = mem.Read<textx>(ActorNamePointer);

					std::wstring test = ActorName.word;
					using convert_type = std::codecvt_utf8<wchar_t>;
					std::wstring_convert<convert_type, wchar_t> converter;
					std::string converted_str = converter.to_bytes(test);

					if (converted_str == menamestring)
						continue;

					info.id = ActorID;
					info.name = converted_str;
					info.type = player;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 100);
					info.yaw = ActorYaw;
					info.health = Actorhealth;
					info.maxhealth = Actormaxhealth;
					ActorArray.push_back(info);
				}

				// --- ITEM FINDING ---

				// Chests
				else if ((hoarders) && ((name.find("BP_TreasureChest_P") != std::string::npos) || (name.find("BP_TreasureChest") != std::string::npos && name.find("Proxy")) || (name.find("BP_ShipwreckTreasureChest_P") != std::string::npos) || (name.find("BP_ShipwreckTreasureChest") != std::string::npos && name.find("Proxy")) || (name.find("StrongholdKey") != std::string::npos)))
				{
					info.type = chest;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 50);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Castaway Chest";
						info.rareity = Common;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Castaway";
						}
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Seafarer Chest";
						info.rareity = Rare;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Seafarer";
						}
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Marauder Chest";
						info.rareity = Legendary;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Marauder";
						}
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Captain Chest";
						info.rareity = Mythical;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Captain";
						}
					}
					if (name.find("Drunken") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Chest of a Thousand Grogs";
						info.rareity = Drunken;
					}
					if (name.find("Weeping") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Chest of Sorrow";
						info.rareity = Weeping;
					}
					if (name.find("Fort") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Stronghold Chest";
						info.rareity = Fort;
					}
					if (name.find("PirateLegend") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Chest of Legends";
						info.rareity = Fort;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Legend Chest";
						}
					}
					if (name.find("StrongholdKey") != std::string::npos || name.find("StrongholdKey") != std::string::npos && name.find("Proxy") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Stronghold Key";
						info.rareity = Fort;
					}
					ActorArray.push_back(info);
				}

				// Skulls
				else if ((orderSouls) && (name.find("BP_BountyRewardSkull_P") != std::string::npos) || (name.find("BP_BountyRewardSkull") != std::string::npos) && (name.find("Proxy") != std::string::npos))
				{
					info.type = skull;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Foul Skull";
						info.rareity = Common;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Foul";
						}
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Disgraced Skull";
						info.rareity = Rare;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Disgraced";
						}
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Hateful Skull";
						info.rareity = Legendary;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Hateful";
						}
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Villainous Skull";
						info.rareity = Mythical;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Villainous";
						}
					}
					if (name.find("Fort") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "StrongHold Skull";
						info.rareity = Fort;
					}
					ActorArray.push_back(info);
				}

				// Merchant Crates
				else if ((merchants) && (name.find("BP_MerchantCrate") != std::string::npos) && (name.find("Proxy") != std::string::npos))
				{
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("PigCrate") != std::string::npos)
					{
						info.type = animalcrate;
						info.id = ActorID;
						info.name = "Pig Crate";
					}
					else if (name.find("SnakeBasket") != std::string::npos)
					{
						info.type = animalcrate;
						info.id = ActorID;
						info.name = "Snake Basket";
					}
					else if (name.find("ChickenCrate") != std::string::npos)
					{
						info.type = animalcrate;
						info.id = ActorID;
						info.name = "Chicken Crate";
					}
					else if (name.find("Gunpowder") != std::string::npos)
					{
						info.type = gunpowder;
						info.id = ActorID;
						info.name = "Gun Powder Barrel";
					}
					else
					{
						info.type = merchantcrate;
						info.id = ActorID;
						if (name.find("TeaCrate") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Tea Crate";
						}
						else if (name.find("SugarCrate") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Sugar Crate";
						}
						else if (name.find("SilkCrate") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Silk Crate";
						}
						else if (name.find("SpiceCrate") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Spice Crate";
						}
						else if (name.find("Gemstones") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Gemstones";
						}
						else if (name.find("Minerals") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Minerals";
						}
						else if (name.find("Ore") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ore";
						}
						else if (name.find("VolcanicStone") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Volcanic Stone";
						}
						else
						{
							info.id = ActorID;
							info.name = "Resource Crate";
						}
					}
					ActorArray.push_back(info);
				}
				// Skeleton Fort - Skull Cloud
				else if (name.find("BP_SkellyFort") != std::string::npos)
				{
					info.type = fort;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("SkullCloud") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Skeleton Fort";
						info.rareity = Legendary;
					}

					ActorArray.push_back(info);
				}
				// Wonder Secrets Box - Not sure if this is the correct name, but its a proxy based chest
				else if (name.find("BP_AncientChest_P") != std::string::npos || name.find("BP_BoxOfSecrets") != std::string::npos || name.find("BP_AncientChest") != std::string::npos && name.find("Proxy") != std::string::npos)
				{
					info.id = ActorID;
					info.type = secretBox;
					info.name = "Box Of Secrets";
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 300);
					info.yaw = ActorYaw;
					ActorArray.push_back(info);
				}
				// Active Volcano - NOT WORKING
				else if (name.find("BP_SuperheatedWater") != std::string::npos)
				{
					info.type = volcano;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					info.id = ActorID;
					info.name = "!ACTIVE VOLCANO!";
					ActorArray.push_back(info);
				}
				// Treasure Artifacts
				else if ((hoarders) && (name.find("BP_TreasureArtifact") != std::string::npos) || (name.find("BP_Treasure_Artifact") != std::string::npos) && (name.find("Proxy") != std::string::npos) || (name.find("BP_TreasureArtifact_Wieldable") != std::string::npos))
				{
					info.type = artifact;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("box") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Shiny Box";
						info.rareity = Common;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Box";
						}
					}
					if (name.find("goblet") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Shiny Goblet";
						info.rareity = Rare;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Goblet";
						}
					}
					if (name.find("impressive") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Shiny Impressive";
						info.rareity = Legendary;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Impressive";
						}
					}
					if (name.find("vase") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Shiny Vase";
						info.rareity = Mythical;
						if (name.find("DVR") != std::string::npos)
						{
							info.id = ActorID;
							info.name = "Ashen Vase";
						}
					}
					if (name.find("base") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Shiny Base";
						info.rareity = Rare;
					}

					ActorArray.push_back(info);
				}
				// Skeletons
				else if (name.find("Skeleton") != std::string::npos)
				{
					info.id = ActorID;
					if (name.find("MetalForm") != std::string::npos)
					{
						info.name = "Metal Skeleton";
						info.rareity = Legendary;
					}
					else if (name.find("ShadowForm") != std::string::npos)
					{
						info.name = "Shadow Skeleton";
						info.rareity = Mythical;
					}
					else if (name.find("PlantForm") != std::string::npos)
					{
						info.name = "Plant Skeleton";
						info.rareity = Rare;
					}
					else
					{
						info.name = "Skeleton";
					}
					info.type = skeleton;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 75);

					ActorArray.push_back(info);
				}
				else if ((merchants) && (name.find("BP_Chicken_") != std::string::npos))
				{
					info.type = chicken;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "White Chicken";
						info.rareity = Common;
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Red Spotted Chicken";
						info.rareity = Rare;
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Black Chicken";
						info.rareity = Legendary;
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Golden Chicken";
						info.rareity = Mythical;
					}

					ActorArray.push_back(info);
				}
				// Pigs
				else if ((merchants) && (name.find("BP_Pig_") != std::string::npos))
				{
					info.type = pig;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Pink Pig";
						info.rareity = Common;
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Rare Pig";
						info.rareity = Rare;
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Black Pig";
						info.rareity = Legendary;
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Golden Pig";
						info.rareity = Mythical;
					}

					ActorArray.push_back(info);
				}
				// Snakes
				else if ((merchants) && (name.find("BP_Snake_") != std::string::npos))
				{
					info.type = snake;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);
					if (name.find("Common") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Common Snake";
						info.rareity = Common;
					}
					if (name.find("Rare") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Rare Snake";
						info.rareity = Rare;
					}
					if (name.find("Legendary") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Black Snake";
						info.rareity = Legendary;
					}
					if (name.find("Mythical") != std::string::npos)
					{
						info.id = ActorID;
						info.name = "Golden Snake";
						info.rareity = Mythical;
					}

					ActorArray.push_back(info);
				}

				// Ships
				else if (name.find("BP_SmallShipNetProxy") != std::string::npos || name.find("BP_MediumShipNetProxy") != std::string::npos || name.find("BP_LargeShipNetProxy") != std::string::npos)
				{
					info.id = ActorID;
					info.name = "Ship";
					info.type = ship;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 300);
					info.yaw = ActorYaw;

					ActorArray.push_back(info);
				}
				else if (name.find("BP_SmallShipTemplate_C") != std::string::npos || name.find("BP_MediumShipTemplate_C") != std::string::npos || name.find("BP_LargeShipTemplate_C") != std::string::npos)
				{
					info.id = ActorID;
					info.name = "Ship";
					info.type = ship;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 300);
					info.yaw = ActorYaw;

					ActorArray.push_back(info);
				}
				// Ghostship
				else if (name.find("BP_GhostShip_C") != std::string::npos || name.find("BP_GhostShip") != std::string::npos && name.find("Proxy") != std::string::npos)
				{
					info.id = ActorID;
					info.type = ghostship;
					info.name = "Ghost Ship";
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 300);
					info.yaw = ActorYaw;

					ActorArray.push_back(info);
				}
				// Ghostship Captain
				else if (name.find("BP_GhostShipCaptain") != std::string::npos)
				{
					info.id = ActorID;
					info.type = ghostcaptain;
					info.name = "Skeleton Captain";
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);

					ActorArray.push_back(info);
				}
				// Shipwrecks
				else if (name.find("BP_Shipwreck_01_a_NetProxy_C") != std::string::npos || name.find("BP_Shipwreck_") != std::string::npos || name.find("BP_Shipwreck") != std::string::npos && name.find("Proxy") != std::string::npos)
				{
					info.id = ActorID;
					info.name = "Shipwreck";
					info.type = animalcrate;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 75);

					ActorArray.push_back(info);
				}
				// Sunken Artefact - Not used anymore?
				else if (name.find("BP_SunkenCurseArtefact_") != std::string::npos)
				{
					info.id = ActorID;
					if (name.find("Ruby") != std::string::npos)
					{
						info.name = "Ruby UnderWater Statue";
					}
					else if (name.find("Emerald") != std::string::npos)
					{
						info.name = "Emerald UnderWater Statue";
					}
					else if (name.find("Sapphire") != std::string::npos)
					{
						info.name = "Sapphire UnderWater Statue";
					}
					info.type = animalcrate;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 75);

					ActorArray.push_back(info);
				}
				// SkellyShip Cloud
				else if (name.find("BP_SkellyShip") != std::string::npos)
				{
					info.id = ActorID;
					info.type = fort;
					info.name = "SkellyShip Cloud";
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);

					ActorArray.push_back(info);
				}
				// Shark
				else if (name.find("BP_Shark") != std::string::npos)
				{
					info.id = ActorID;
					info.type = shark;
					info.name = "Shark";
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);

					ActorArray.push_back(info);
				}
				// Mermaid
				else if (name.find("BP_Mermaid") != std::string::npos)
				{
					info.id = ActorID;
					info.type = mermaid;
					info.name = "Mermaid";
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);

					ActorArray.push_back(info);
				}
				// Barrel Seagulls
				else if (name.find("BP_Seagulls_Barrels") != std::string::npos)
				{
					info.id = ActorID;
					info.type = barrels;
					info.name = "Barrel Seagulls";
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);

					ActorArray.push_back(info);
				}
				// Hide unwanted items
				else if ((btDebug) && ((name.find("BP_AthenaPlayerCameraManager") != std::string::npos) || (name.find("BP_Buoyant") != std::string::npos) || (name.find("BP_OnlineAthenaPlayerController") != std::string::npos)))
				{}
				// Debug
				else if (btDebug)
				{
					info.id = ActorID;
					info.name = name;
					info.Location = Actorrelativelocation;
					info.TopLocation = Vector3(Actorrelativelocation.x, Actorrelativelocation.y, Actorrelativelocation.z + 10);

					ActorArray.push_back(info);
				}
				if (IslandDataAsset_PTR != 0)
				{
					if (name == "BP_TreasureMap_C")
					{
						std::string MapTexturePath;
						std::vector<Vector2> Marks;
						if (get_TreasureMap(Actor, &MapTexturePath, &Marks))
						{
							DWORD_PTR TreasureLocations_PTR = 0;
							__int32 TreasureLocations_Count = 0;

							if (find_Island_In_IslandDataEntries(
								MapTexturePath, &TreasureLocations_PTR, &TreasureLocations_Count))
							{
								for (auto const& value : Marks)
								{
									for (int nIndex = 0; nIndex < TreasureLocations_Count; nIndex++)
									{
										const FTreasureLocationData cTreasureLocation
											= mem.Read<FTreasureLocationData>(TreasureLocations_PTR
												+ (nIndex * sizeof(FTreasureLocationData)));
										if (cTreasureLocation.MapSpaceLocation.x == value.x
											&& cTreasureLocation.MapSpaceLocation.y == value.y)
										{
											new_XMarksTheSpot.push_back(cTreasureLocation.WorldSpaceLocation);
											break;
										}
									}
								}
							}
						}
					}
				}
			}


			// Xmarks items - NOT WORKING 
			XMarksTheSpot = new_XMarksTheSpot;
			myLocation = mem.Read<Vector3>(RootComponent + Offsets::RelativeLocation);
			myAngles = mem.Read<Vector3>(CameraManager + Offsets::CameraRotation);
			Cameralocation = mem.Read<Vector3>(CameraManager + Offsets::CameraLocation);
			CameraFov = mem.Read<float>(CameraManager + Offsets::DefaultFOV);
			//auto myhealth = mem.Read<float>(HealthComponet + Offsets::CurrentHealth);
			//auto maxhealth = mem.Read<float>(HealthComponet + Offsets::MaxHealth);
			Sleep(20);

			// --- ESP DRAWING ---

			// "XMarksTheSpot"
			for (int i = 0; i < XMarksTheSpot.size(); i++)
			{
				Vector2 ScreenPoints;
				if (WorldToScreen(XMarksTheSpot.at(i), &ScreenPoints))
					DrawString((char*)"X", ScreenPoints.x, ScreenPoints.y, 255, 0, 0, pFontSmall);
			}

			// Crosshair
			FillRGB((Width / 2), (Height / 2) - 15, 1, 30, 255, 40, 40, 120);
			FillRGB((Width / 2) -15, (Height / 2), 30, 1, 255, 40, 40, 120);

			// Radar ESP
			FillRGB(180, 30, 1, 300, 75, 68, 67, 90);
			FillRGB(30, 180, 300, 1, 75, 68, 67, 90);

			for (int i = 0; i < ActorArray.size(); i++)
			{
				float angle = getAngle(Vector2(myLocation.x, myLocation.y), Vector2(ActorArray.at(i).Location.x, ActorArray.at(i).Location.y));
				Vector2 dist = Vector2((myLocation.x - ActorArray.at(i).Location.x) / 100, (myLocation.y - ActorArray.at(i).Location.y) / 100);
				int distance = Vector2(myLocation.x, myLocation.y).DistTo(Vector2(ActorArray.at(i).Location.x, ActorArray.at(i).Location.y)) / 100;

				// Draw distance (in meters) of Actor from player
				ActorArray.at(i).name = ActorArray.at(i).name + " [" + std::to_string(distance) + "m]";
				Vector2 point2dist = me - dist;
				Vector2 ScreenPoint = RotatePoint(point2dist, me, -myAngles.y - 90, false);

				if (ScreenPoint.x < 30)
					ScreenPoint.x = 30;
				if (ScreenPoint.y < 30)
					ScreenPoint.y = 30;
				if (ScreenPoint.x > 330)
					ScreenPoint.x = 330;
				if (ScreenPoint.y > 330)
					ScreenPoint.y = 330;

				// Volcano ESP
				else if (ActorArray.at(i).type == volcano)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 125, 0, 255); //ORANGE
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 165, 42, 42, pFontSmall);
				}
				// Box Of Secrets ESP
				else if (ActorArray.at(i).type == secretBox) 
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 157, 0, 255, 255); // PURPLE
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 230, 230, 250, pFontSmall);
				}
				// Player Ship ESP
				else if (ActorArray.at(i).type == ship)
				{
					FillRGB(ScreenPoint.x - 5, ScreenPoint.y - 5, 10, 10, 0, 255, 0, 255); // LIME GREEN
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString((char*)ActorArray.at(i).name.c_str(), ScreenPoint.x, ScreenPoint.y, 0, 255, 0, pFontSmall);
				}
				// Ghost Ship ESP
				else if (ActorArray.at(i).type == ghostship)
				{
					FillRGB(ScreenPoint.x - 5, ScreenPoint.y - 5, 10, 10, 125, 255, 0, 255); // YELLOW-GREEN
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString((char*)ActorArray.at(i).name.c_str(), ScreenPoint.x, ScreenPoint.y, 0, 255, 100, pFontSmall);
				}
				// Player ESP
				else if (ActorArray.at(i).type == player)
				{
						FillRGB(ScreenPoint.x - 3, ScreenPoint.y - 3, 6, 6, 0, 0, 255, 255); // BLUE
						Vector2 headpoint;

						if (WorldToScreen(ActorArray.at(i).TopLocation, &headpoint) && WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						{
							int hi = (ScreenPoint.y - headpoint.y) * 2;
							int wi = hi * 0.65;

							DrawBox(headpoint.x - wi / 2, headpoint.y, wi, hi, 1, 0, 0, 255, 255);

							int health = ActorArray.at(i).health;
							if (health > 100)
								health = 100;
							int r = 255 - health * 2.55;
							int g = health * 2.55;
							float wtf = health / 100.f;
							float healthBar = hi * wtf;
							int healthBarDelta = hi - healthBar;

							FillRGB(headpoint.x - wi / 2 - 6, headpoint.y - 1, 5, hi + 2, 0, 0, 0, 255);
							FillRGB(headpoint.x - wi / 2 - 5, headpoint.y + healthBarDelta, 3, healthBar, r, g, 0, 255);

							DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), headpoint.x - (GetTextWidth(ActorArray.at(i).name.c_str(), pFontSmall) / 2), headpoint.y - 14, 255, 255, 255, pFontSmall);
							DrawString(const_cast<char*>(ActorArray.at(i).item.c_str()), headpoint.x - (GetTextWidth(ActorArray.at(i).item.c_str(), pFontSmall) / 2), headpoint.y + hi, 255, 255, 255, pFontSmall);
						}

				}
				// Animal Crate ESP
				else if (ActorArray.at(i).type == animalcrate)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 230, 230, 250, 255); // LIGHT PURPLE
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 230, 230, 250, pFontSmall);
				}
				// Gunpowder ESP
				else if (ActorArray.at(i).type == gunpowder)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 0, 0, 255); // RED
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 255, 0, 0, pFontSmall);
				}
				// Merchant Crate ESP
				else if (ActorArray.at(i).type == merchantcrate)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 165, 0, 255); // ORANGE
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 255, 165, 0, pFontSmall);
				}
				// Skeleton ESP
				else if (ActorArray.at(i).type == skeleton)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 0, 0, 255); // RED
					Vector2 headpoint;

					if (WorldToScreen(ActorArray.at(i).TopLocation, &headpoint) && WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
					{
						int hi, wi;
						hi = (ScreenPoint.y - headpoint.y) * 2;
						wi = hi * 0.65;

						DrawBox(headpoint.x - wi / 2, headpoint.y, wi, hi, 1, 255, 0, 0, 255); // RED
					}
				}
				// Skeleton Ship Captain ESP
				else if (ActorArray.at(i).type == ghostcaptain)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 0, 0, 255); // RED
					Vector2 headpoint;
					if (WorldToScreen(ActorArray.at(i).TopLocation, &headpoint) && WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
					{
						int hi, wi;
						hi = (ScreenPoint.y - headpoint.y) * 2;
						wi = hi * 0.65;

						DrawBox(headpoint.x - wi / 2, headpoint.y, wi, hi, 1, 255, 100, 100, 255);
					}
				}
				// Rareity ESP
				else if (ActorArray.at(i).rareity == Common)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 165, 42, 42, 255); // MARROON? Idk my colors, a blood red almost? 
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 165, 42, 42, pFontSmall);
				}
				else if (ActorArray.at(i).rareity == Rare)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 0, 255, 255, 255); // LIGHT BLUE
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 0, 255, 255, pFontSmall);
				}
				else if (ActorArray.at(i).rareity == Legendary)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 105, 180, 255); // PINK
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 255, 105, 180, pFontSmall);
				}
				else if (ActorArray.at(i).rareity == Mythical)
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 75, 0, 130, 255); // PURPLE
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 75, 0, 130, pFontSmall);
				}
				// All Other ESP
				else
				{
					FillRGB(ScreenPoint.x - 2, ScreenPoint.y - 2, 4, 4, 255, 215, 0, 255); // YELLOW
					if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
						DrawString(const_cast<char*>(ActorArray.at(i).name.c_str()), ScreenPoint.x, ScreenPoint.y, 255, 215, 05, pFontSmall);
				}

				ActorArray.erase(ActorArray.begin() + i);
			}
	}

	// ESP Button Control
	if (GetAsyncKeyState(VK_F9) & 1) // Exit ESP
	{
		exit(1);
	}
	else if (GetAsyncKeyState(VK_F8) & 1) // Debug Items
	{
		btDebug = false;
	}
	else if (GetAsyncKeyState(VK_F7) & 1) // Debug Items Off
	{
		btDebug = true;
	}
	else if (GetAsyncKeyState(VK_F6) & 1) // Hoarders Items On
	{
		hoarders = true;
	}
	else if (GetAsyncKeyState(VK_F5) & 1) // Order Of Souls Items On
	{
		orderSouls = true;
	}
	else if (GetAsyncKeyState(VK_F4) & 1) // Order Of Souls Items On
	{
		merchants = true;
	}
	else if (GetAsyncKeyState(VK_F3) & 1) // All Rep Items Off
	{
		orderSouls = false;
		merchants = false;
		hoarders = false;
	}

	p_Device->EndScene();
	p_Device->PresentEx(0, 0, 0, 0, 0);
	return 0;
}
#include "Functions.h"
#include "Offsets.h"

int width = 1920, height = 1080;
char lWindowName[256] = "Overlay";
char tWindowName[256] = "Sea of Thieves";
HWND hWnd, tWnd;
MSG message;
Vector2 me = Vector2(180, 180);
bool btDebug, btAnimals;

ID2D1Factory *pFactory;
ID2D1HwndRenderTarget *pRenderTarget;
ID2D1SolidColorBrush* pBrush;
IDWriteTextFormat* pTextFormat;
IDWriteFactory* pDWriteFactory;
RECT hWindow;
RECT tWindow;

/*
// TBA Objects Of Interest From SDK data mining /Aepko

- Skeleton Lord
- Banjo
- Boarding Axe
- Bread
- Coconut
- Concertina
- Dice
- Fiddle
- FishingRod
- FishMeat
- HeavySword
- Mango
- Pet_Cat
- Pet_Monkey
- Pet_Parrot
- Pet_Pig
- Pineapple
- Pipe
- Pomegranate
- RapierSword
- ScrubBrush
- SharkMeat
*/


int render()
{
	pRenderTarget->BeginDraw();
	pRenderTarget->Clear();

	tWnd = FindWindow(0, tWindowName);
	if (tWnd)
	{
		GetWindowRect(tWnd, &tWindow);
		MoveWindow(hWnd, tWindow.left, tWindow.top, tWindow.right - tWindow.left, tWindow.bottom - tWindow.top, true);
		pRenderTarget->Resize(D2D1::SizeU(tWindow.right, tWindow.bottom));

		ReadData();
		auto World = mem.Read<ULONG_PTR>(WorldAddress);
		GNames = mem.Read<ULONG_PTR>(NamesAddress);
		ULONG_PTR owninggameinstance = mem.Read<ULONG_PTR>(World + Offsets::OwningGameInstance);
		ULONG_PTR LocalPlayers = mem.Read<ULONG_PTR>(owninggameinstance + Offsets::LocalPlayers);
		ULONG_PTR ULocalPlayer = mem.Read<ULONG_PTR>(LocalPlayers);
		ULONG_PTR PlayerController = mem.Read<ULONG_PTR>(ULocalPlayer + Offsets::PlayerController);
		ULONG_PTR ULevel = mem.Read<ULONG_PTR>(World + Offsets::PersistentLevel);
		auto ActorCount = mem.Read<int>(ULevel + Offsets::ActorsTArrayCount);
		auto LocalPlayer = mem.Read<ULONG_PTR>(PlayerController + Offsets::Pawn);
		auto LocalPlayeState = mem.Read<ULONG_PTR>(PlayerController + Offsets::PlayerState);
		auto HealthComponet = mem.Read<ULONG_PTR>(LocalPlayer + Offsets::HealthComponent);
		auto CameraManager = mem.Read<ULONG_PTR>(PlayerController + Offsets::PlayerCameraManager);
		auto RootComponent = mem.Read<ULONG_PTR>(LocalPlayer + Offsets::RootComponent);
		std::vector<Vector3> new_XMarksTheSpot;

		for (int i = 0; i < ActorCount; i++)
		{
			ULONG_PTR ActorList = mem.Read<ULONG_PTR>(ULevel + Offsets::ActorsTArray);
			ULONG_PTR Actor = mem.Read<ULONG_PTR>(ActorList + (i * 0x8));

			if (!Actor)
				continue;

			int ActorID = mem.Read<int>(Actor + Offsets::Id);
			auto ActorRootComponent = mem.Read<ULONG_PTR>(Actor + Offsets::RootComponent);
			auto Actorrelativelocation = mem.Read<Vector3>(ActorRootComponent + Offsets::RelativeLocation);
			std::string name = GetActorName(ActorID);

		/*	if (name.find("BP_") == std::string::npos)
				continue;
		*/
			AActors info;

			/*
			// Player Names
			if (name.find("BP_PlayerPirate_C") != std::string::npos)
			{
				const auto ActorPlayerstate = mem.Read<ULONG_PTR>(Actor + Offsets::PlayerState);
				const auto ActorNamePointer = mem.Read<ULONG_PTR>(ActorPlayerstate + Offsets::PlayerName);
				const auto ActorName = mem.Read<textx>(ActorNamePointer);

				info.id = ActorID;
				info.name = converted_str;
				info.type = player;
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			*/
			// Ships
			if (name.find("BP_SmallShipNetProxy") != std::string::npos || name.find("BP_MediumShipNetProxy") != std::string::npos || name.find("BP_LargeShipNetProxy") != std::string::npos)
			{
				info.id = ActorID;
				info.name = "Ship";
				info.type = ship;
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			else if (name.find("BP_SmallShipTemplate_C") != std::string::npos || name.find("BP_MediumShipTemplate_C") != std::string::npos || name.find("BP_LargeShipTemplate_C") != std::string::npos)
			{
				info.id = ActorID;
				info.name = "Ship";
				info.type = ship;
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Chests
			else if (name.find("BP_TreasureChest_") != std::string::npos || name.find("BP_TreasureChest") != std::string::npos && name.find("Proxy") || name.find("StrongholdKey") != std::string::npos)
			{
				info.id = ActorID;
				if (name.find("Common") != std::string::npos || name.find("Common-") != std::string::npos || name.find("Common+") != std::string::npos)
				{
					info.name = "Castaway Chest";
					info.type = common;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Castaway";
					}
				}
				if (name.find("Rare") != std::string::npos || name.find("Rare-") != std::string::npos || name.find("Rare+") != std::string::npos)
				{
					info.name = "Seafarer Chest";
					info.type = rare;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Seafarer";
					}
				}
				if (name.find("Legendary") != std::string::npos || name.find("Legendary-") != std::string::npos || name.find("Legendary+") != std::string::npos)
				{
					info.name = "Marauder Chest";
					info.type = legendary;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Marauder";
					}
				}
				if (name.find("Mythical") != std::string::npos || name.find("Mythical-") != std::string::npos || name.find("Mythical+") != std::string::npos)
				{
					info.name = "Captain Chest";
					info.type = mythical;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Captain";
					}
				}
				if (name.find("Drunken") != std::string::npos || name.find("Drunken-") != std::string::npos || name.find("Drunken+") != std::string::npos)
				{
					info.name = "Chest of a Thousand Grogs";
					info.type = mythical;
				}
				if (name.find("Weeping") != std::string::npos || name.find("Weeping-") != std::string::npos || name.find("Weeping+") != std::string::npos)
				{
					info.name = "Chest of Sorrow";
					info.type = mythical;
				}
				if (name.find("Fort") != std::string::npos)
				{
					info.name = "Stronghold Chest";
					info.type = mythical;
				}
				if (name.find("PirateLegend") != std::string::npos)
				{
					info.name = "Chest of Legends";
					info.type = mythical;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Legend Chest";
					}
				}
				if (name.find("StrongholdKey") != std::string::npos || name.find("StrongholdKey") != std::string::npos && name.find("Proxy") != std::string::npos)
				{
					info.name = "Stronghold Key";
					info.type = mythical;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Shipwreck Chests
			else if (name.find("BP_ShipwreckTreasureChest_") != std::string::npos || name.find("BP_ShipwreckTreasureChest_") != std::string::npos && name.find("Proxy_") || name.find("BP_BP_ShipwreckTreasureChest_Wieldable_") != std::string::npos)
			{
				info.id = ActorID;
				if (name.find("Common") != std::string::npos)
				{
					info.name = "Sunken Castaway";
					info.type = common;
				}
				if (name.find("Rare") != std::string::npos)
				{
					info.name = "Sunken Seafarer";
					info.type = rare;
				}
				if (name.find("Legendary") != std::string::npos)
				{
					info.name = "Sunken Marauder";
					info.type = legendary;
				}
				if (name.find("Mythical") != std::string::npos)
				{
					info.name = "Sunken Captain";
					info.type = mythical;
				}
				if (name.find("Drunken") != std::string::npos)
				{
					info.name = "Thousand Grogs";
					info.type = mythical;
				}
				if (name.find("Weeping") != std::string::npos)
				{
					info.name = "Chest of Sorrow";
					info.type = mythical;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Test To Find Non-Showing Items
			else if (name.find("ALK_WieldableObject") != std::string::npos)
			{
			info.id = ActorID;
				if (name.find("ArtifactBox") != std::string::npos)
				{
					info.name = "!TEST! ArtifactBox";
					info.type = common;
				}
				if (name.find("ArtifactGoblet") != std::string::npos)
				{
					info.name = "!TEST! ArtifactGoblet";
					info.type = common;
				}
				if (name.find("ArtifactImpressive") != std::string::npos)
				{
					info.name = "!TEST! ArtifactImpressive";
					info.type = common;
				}
				if (name.find("ArtifactVase") != std::string::npos)
				{
					info.name = "!TEST! ArtifactVase";
					info.type = common;
				}
				if (name.find("GunpowderBarrel") != std::string::npos)
				{
					info.name = "!TEST! GunpowderBarrel";
					info.type = common;
				}
				if (name.find("Skull") != std::string::npos)
				{
					info.name = "!TEST! Skull";
					info.type = common;
				}
				if (name.find("TreasureChest") != std::string::npos)
				{
					info.name = "!TEST! TreasureChest";
					info.type = common;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Skulls
			else if (name.find("BP_BountyRewardSkull_Proxy_") != std::string::npos || name.find("BP_BountyRewardSkull_") != std::string::npos)
			{
				info.id = ActorID;
				if (name.find("Common") != std::string::npos || name.find("Common-") != std::string::npos || name.find("Common+") != std::string::npos)
				{
					info.name = "Foul Skull";
					info.type = common;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Foul";
					}
				}
				if (name.find("Rare") != std::string::npos || name.find("Rare-") != std::string::npos || name.find("Rare+") != std::string::npos)
				{
					info.name = "Disgraced Skull";
					info.type = rare;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Disgraced";
					}
				}
				if (name.find("Legendary") != std::string::npos || name.find("Legendary-") != std::string::npos || name.find("Legendary+") != std::string::npos)
				{
					info.name = "Hateful Skull";
					info.type = legendary;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Hateful";
					}
				}
				if (name.find("Mythical") != std::string::npos || name.find("Mythical-") != std::string::npos || name.find("Mythical+") != std::string::npos)
				{
					info.name = "Villainous Skull";
					info.type = mythical;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Villainous";
					}
				}
				if (name.find("Fort") != std::string::npos)
				{
					info.name = "StrongHold Skull";
					info.type = mythical;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			else if (name.find("BP_MerchantCrate_") != std::string::npos)
			{
				info.id = ActorID;
				if (name.find("PigCrate") != std::string::npos || name.find("PigCrateProxy") != std::string::npos)
				{
					info.type = crate;
					info.name = "Pig Crate";
				}
				else if (name.find("SnakeBasket") != std::string::npos)
				{
					info.type = crate;
					info.name = "Snake Basket";
				}
				else if (name.find("ChickenCrate") != std::string::npos)
				{
					info.type = crate;
					info.name = "Chicken Crate";
				}
				else if (name.find("GunPowderBarrel") != std::string::npos)
				{
					info.type = other;
					info.name = "Gun Powder Barrel";
				}
				else
				{
					info.type = crate;
					if (name.find("Commodity_TeaCrate") != std::string::npos)
					{
						info.name = "Tea Crate";
					}
					else if (name.find("Commodity_SugarCrate") != std::string::npos)
					{
						info.name = "Sugar Crate";
					}
					else if (name.find("Commodity_SilkCrate") != std::string::npos)
					{
						info.name = "Silk Crate";
					}
					else if (name.find("Commodity_SpiceCrate") != std::string::npos)
					{
						info.name = "Spice Crate";
					}
					else if (name.find("Commodity_Gemstones") != std::string::npos || name.find("Commodity_Gemstones_Proxy") != std::string::npos)
					{
						info.name = "Gemstones";
					}
					else if (name.find("Commodity_Minerals") != std::string::npos || name.find("Commodity_Minerals_Proxy") != std::string::npos)
					{
						info.name = "Minerals";
					}
					else if (name.find("Commodity_Ore") != std::string::npos || name.find("Commodity_Ore_Proxy") != std::string::npos)
					{
						info.name = "Ore";
					}
					else if (name.find("Commodity_VolcanicStone") != std::string::npos || name.find("Commodity_VolcanicStone_Proxy") != std::string::npos)
					{
						info.name = "Volcanic Stone";
					}
					else if (name.find("WoodCrate") != std::string::npos || name.find("WoodCrateProxy") != std::string::npos)
					{
						info.name = "Wood Crate";
					}
					else if (name.find("CannonBallCrate") != std::string::npos || name.find("CannonBallCrateProxy") != std::string::npos)
					{
						info.name = "Cannon Ball Crate";
					}
					else if (name.find("BananaCrate") != std::string::npos || name.find("BananaCrateProxy") != std::string::npos)
					{
						info.name = "Banana Crate";
					}
					else
					{
						info.name = "Resource Crate";
					}
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Artifacts
			else if (name.find("BP_TreasureArtifact") != std::string::npos || name.find("BP_Treasure_Artifact") != std::string::npos && name.find("Proxy") != std::string::npos || name.find("BP_TreasureArtifact_Wieldable") != std::string::npos)
			{
				info.id = ActorID;
				if (name.find("box") != std::string::npos)
				{
					info.name = "Shiny Box";
					info.type = common;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Box";
					}
				}
				if (name.find("goblet") != std::string::npos)
				{
					info.name = "Shiny Goblet";
					info.type = rare;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Goblet";
					}
				}
				if (name.find("impressive") != std::string::npos)
				{
					info.name = "Shiny Impressive";
					info.type = legendary;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Impressive";
					}
				}
				if (name.find("vase") != std::string::npos)
				{
					info.name = "Shiny Vase";
					info.type = mythical;
					if (name.find("DVR") != std::string::npos)
					{
						info.name = "Ashen Vase";
					}
				}
				if (name.find("base") != std::string::npos)
				{
					info.name = "Shiny Base";
					info.type = rare;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Merchant Cargo
			else if (name.find("BP_CargoRunCrate_") != std::string::npos)
			{
			info.id = ActorID;
			if (name.find("Cloth") != std::string::npos)
			{
				info.name = "Cloth Cargo";
				info.type = crate;
				if (name.find("DVR") != std::string::npos)
				{
					info.name = "Devils Cloth";
				}
			}
			if (name.find("Plants") != std::string::npos)
			{
				info.name = "Plants Cargo";
				info.type = crate;
				if (name.find("DVR") != std::string::npos)
				{
					info.name = "Devils Plants";
				}
			}
			if (name.find("Rum") != std::string::npos)
			{
				info.name = "Rum Cargo";
				info.type = crate;
				if (name.find("DVR") != std::string::npos)
				{
					info.name = "Devils Rum";
				}
			}
			info.Location = Actorrelativelocation;
			ActorArray.push_back(info);
			}
			// Box Of Secrets
			else if (name.find("BP_AncientChest_") != std::string::npos || name.find("BP_BoxOfSecrets") != std::string::npos || name.find("BP_AncientChest_") != std::string::npos && name.find("Proxy") != std::string::npos)
			{
				info.id = ActorID;
				info.type = mythical;
				info.name = "Box Of Secrets";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Ghostship
			else if (name.find("BP_GhostShip_C") != std::string::npos || name.find("BP_GhostShip") != std::string::npos && name.find("Proxy") != std::string::npos)
			{
				info.id = ActorID;
				info.type = ship;
				info.name = "Ghost Ship";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Ghostship Captain
			else if (name.find("BP_GhostShipCaptain") != std::string::npos)
			{
				info.id = ActorID;
				info.type = enemy;
				info.name = "Skeleton Captain";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Skeletons
			else if (name.find("BP_MetalAIFormComponent") != std::string::npos)
			{
				info.id = ActorID;
				info.name = "Metal Skeleton";
				info.type = enemy;
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			else if (name.find("BP_PlantAIFormComponent") != std::string::npos)
			{
				info.id = ActorID;
				info.name = "Plant Skeleton";
				info.type = enemy;
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			else if (name.find("BP_ShadowAIFormComponent") != std::string::npos)
			{
				info.id = ActorID;
				info.name = "Shadow Skeleton";
				info.type = enemy;
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			else if (name.find("BP_BoneForm") != std::string::npos)
			{
			info.id = ActorID;
			info.name = "Skeleton";
			info.type = enemy;
			info.Location = Actorrelativelocation;
			ActorArray.push_back(info);
			}
			// Skeleton Fort Cloud
			else if (name.find("BP_SkellyFort") != std::string::npos)
			{
				info.id = ActorID;
				info.type = cloud;
				info.name = "Skeleton Fort Cloud";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Skeleton Ship Cloud
			else if (name.find("BP_SkellyShip") != std::string::npos)
			{
				info.id = ActorID;
				info.type = cloud;
				info.name = "Skeleton Ship Cloud";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Chickens
			else if ((name.find("BP_Chicken_") != std::string::npos) && (btAnimals))
			{
				info.id = ActorID;
				if (name.find("Common") != std::string::npos)
				{
					info.name = "White Chicken";
					info.type = common;
				}
				if (name.find("Rare") != std::string::npos)
				{
					info.name = "Red Spotted Chicken";
					info.type = rare;
				}
				if (name.find("Legendary") != std::string::npos)
				{
					info.name = "Black Chicken";
					info.type = legendary;
				}
				if (name.find("Mythical") != std::string::npos)
				{
					info.name = "Golden Chicken";
					info.type = mythical;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Pigs
			else if ((name.find("BP_Pig_") != std::string::npos) && (btAnimals))
			{
				info.id = ActorID;
				if (name.find("Common") != std::string::npos)
				{
					info.name = "Pink Pig";
					info.type = common;
				}
				if (name.find("Rare") != std::string::npos)
				{
					info.name = "Rare Pig";
					info.type = rare;
				}
				if (name.find("Legendary") != std::string::npos)
				{
					info.name = "Black Pig";
					info.type = legendary;
				}
				if (name.find("Mythical") != std::string::npos)
				{
					info.name = "Golden Pig";
					info.type = mythical;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Snakes
			else if ((name.find("BP_Snake_") != std::string::npos) && (btAnimals))
			{
				info.id = ActorID;
				if (name.find("Common") != std::string::npos)
				{
					info.name = "Common Snake";
					info.type = common;
				}
				if (name.find("Rare") != std::string::npos)
				{
					info.name = "Rare Snake";
					info.type = rare;
				}
				if (name.find("Legendary") != std::string::npos)
				{
					info.name = "Black Snake";
					info.type = legendary;
				}
				if (name.find("Mythical") != std::string::npos)
				{
					info.name = "Golden Snake";
					info.type = mythical;
				}
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Shipwrecks
			else if (name.find("BP_Shipwreck_01_a_NetProxy_C") != std::string::npos || name.find("BP_Shipwreck_") != std::string::npos || name.find("BP_Shipwreck") != std::string::npos && name.find("Proxy") != std::string::npos)
			{
				info.id = ActorID;
				info.name = "Shipwreck";
				info.type = crate;
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Mysterious Journal
			else if (name.find("BP_LoreBook") != std::string::npos)
			{
				info.id = ActorID;
				info.type = rare;
				info.name = "Mysterious Journal";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Message In A Bottle
			else if (name.find("BP_MessageInABottle_") != std::string::npos)
			{
			info.id = ActorID;
			info.type = rare;
			info.name = "Message Bottle";
			info.Location = Actorrelativelocation;
			ActorArray.push_back(info);
			}
			// Barrel Seagulls
			else if (name.find("BP_Seagulls_Barrels") != std::string::npos)
			{
				info.id = ActorID;
				info.type = other;
				info.name = "Barrel Seagulls";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Rowboat
			else if (name.find("BP_Rowboat") != std::string::npos)
			{
				info.id = ActorID;
				info.type = limited;
				info.name = "Rowboat";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Mermaid
			else if (name.find("BP_Mermaid") != std::string::npos)
			{
				info.id = ActorID;
				info.type = limited;
				info.name = "Mermaid";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Shark
			else if (name.find("BP_Shark") != std::string::npos)
			{
				info.id = ActorID;
				info.type = limited;
				info.name = "Shark";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// Megalodon
			else if (name.find("BP_TinyShark") != std::string::npos)
			{
				info.id = ActorID;
				info.type = limited;
				info.name = "Megalodon";
				info.Location = Actorrelativelocation;
				ActorArray.push_back(info);
			}
			// X Marks The Spot
			else if (name.find("IslandService") != std::string::npos)
				{
				IslandDataAsset_PTR = mem.Read<ULONG_PTR>(Actor + Offsets::IslandDataAsset);
				}

				if (IslandDataAsset_PTR != NULL)
				{
					if (name == "BP_TreasureMap_C")
					{
						std::string MapTexturePath;
						std::vector<Vector2> Marks;
						if (get_TreasureMap(Actor, &MapTexturePath, &Marks))
						{
							DWORD_PTR TreasureLocations_PTR = NULL;
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
											new_XMarksTheSpot.push_back(
												cTreasureLocation.WorldSpaceLocation);
											break;
										}
									}
								}
							}
						}
					}
				}
				/*else if (name.find("GameState") != std::string::npos)
			{
				info.id = ActorID;
				info.name = name;//"AIslandService";


				//IslandDataAsset_PTR = mem.Read<ULONG_PTR>(Actor + 0x4d0);

				ActorArray.push_back(info);
			}*/

			}

			XMarksTheSpot = new_XMarksTheSpot;

		myLocation = mem.Read<Vector3>(RootComponent + Offsets::RelativeLocation);
		myAngles = mem.Read<Vector3>(CameraManager + Offsets::CameraRotation);
		CameraCachePOV = mem.Read<Vector3>(CameraManager + Offsets::CameraCachePOV);
		DefaultFOV = mem.Read<float>(CameraManager + Offsets::DefaultFOV);
		auto myhealth = mem.Read<float>(HealthComponet + Offsets::CurrentHealth);
		auto maxhealth = mem.Read<float>(HealthComponet + Offsets::MaxHealth);

		for (int i = 0; i < XMarksTheSpot.size(); i++)
		{
			Vector2 ScreenPoint;
			int distance = Vector2(myLocation.x, myLocation.y).DistTo(Vector2(ActorArray.at(i).Location.x, ActorArray.at(i).Location.y)) / 100;
			if (WorldToScreen(XMarksTheSpot.at(i), &ScreenPoint))
				pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &pBrush);
		}
		// End X Marks The Spot

		for (int i = 0; i < ActorArray.size(); i++)
		{
			float angle = getAngle(Vector2(myLocation.x, myLocation.y), Vector2(ActorArray.at(i).Location.x, ActorArray.at(i).Location.y));
			Vector2 dist = Vector2((myLocation.x - ActorArray.at(i).Location.x) / 100, (myLocation.y - ActorArray.at(i).Location.y) / 100);
			int distance = Vector2(myLocation.x, myLocation.y).DistTo(Vector2(ActorArray.at(i).Location.x, ActorArray.at(i).Location.y)) / 100;

			// Draw distance of Actor from player
			ActorArray.at(i).name = ActorArray.at(i).name + " [" + std::to_string(distance) + "m]";
			Vector2 point2dist = me - dist;
			Vector2 ScreenPoint = RotatePoint(point2dist, me, -myAngles.y - 90, false);

			if (WorldToScreen(ActorArray.at(i).Location, &ScreenPoint))
			{
				if (ActorArray.at(i).type == player)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Firebrick, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == ship)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LimeGreen, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == common)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == rare)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Purple, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == legendary)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == mythical)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == cloud)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SlateBlue, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == crate)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Silver, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == enemy)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == limited)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue, 1.0f), &pBrush);
				else if (ActorArray.at(i).type == other)
					pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Olive, 1.0f), &pBrush);

				std::wstring wide(ActorArray.at(i).name.begin(), ActorArray.at(i).name.end());
				pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(ScreenPoint.x, ScreenPoint.y));
				pRenderTarget->DrawText(wide.c_str(), wide.size(), pTextFormat, D2D1::RectF(hWindow.right, hWindow.bottom, 0, 0), pBrush);
			}
			ActorArray.erase(ActorArray.begin() + i);
		}
	}

	// Crosshair
	pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DimGray, 1.0f), &pBrush);
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	pRenderTarget->DrawLine(D2D1::Point2F(hWindow.right / 2, hWindow.bottom / 2 - 15), D2D1::Point2F(hWindow.right / 2, hWindow.bottom / 2 + 15), pBrush);
	pRenderTarget->DrawLine(D2D1::Point2F(hWindow.right / 2 - 15, hWindow.bottom / 2), D2D1::Point2F(hWindow.right / 2 + 15, hWindow.bottom / 2), pBrush);
	pRenderTarget->EndDraw();

	// F9 Exit, F8 Debug, F7 Animals
	if (GetAsyncKeyState(VK_F9) & 1)
	{
		exit(1);
	}
	else if (GetAsyncKeyState(VK_F8) & 1)
	{
		if (btDebug)
			btDebug = false;
		else if (!btDebug)
			btDebug = true;
	}
	else if (GetAsyncKeyState(VK_F7) & 1)
	{
		if (btAnimals)
			btAnimals = false;
		else if (!btAnimals)
			btAnimals = true;
	}

	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
		GetClientRect(hWnd, &hWindow);
		pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(hWindow.right, hWindow.bottom)), &pRenderTarget);
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pDWriteFactory), reinterpret_cast<IUnknown **>(&pDWriteFactory));
		pDWriteFactory->CreateTextFormat(L"Arial", 0, DWRITE_FONT_WEIGHT_THIN, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12, L"", &pTextFormat);
		return 0;

	case WM_PAINT:
		render();
		return 0;

	case WM_DESTROY:
		exit(1);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hSecInstance, LPSTR nCmdLine, INT nCmdShow)
{
	WNDCLASS wclass = {CS_HREDRAW | CS_VREDRAW, WinProc, 0, 0, hInstance, 0, LoadCursor(0, IDC_ARROW), 0, 0, lWindowName};
	RegisterClass(&wclass);

	hWnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST, lWindowName, lWindowName, WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, 0, 0);
	SetLayeredWindowAttributes(hWnd, 0, 0, LWA_COLORKEY);
	ShowWindow(hWnd, SW_SHOW);

	while (GetMessage(&message, hWnd, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return 0;
}
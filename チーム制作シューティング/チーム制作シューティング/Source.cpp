#include <string>
#include "XLibrary11.hpp"
using namespace XLibrary;

enum Mode
{
	Title,
	Game,
	Gameover,
	Gameclear,
};
//ランダムな座標を取得する
Float3 GetRandomPosition()
{
	float range = rand() / (float)RAND_MAX *
		Window::GetSize().x + 500.0f;
	float radian = rand() / (float)RAND_MAX*
		XM_PI * 2.0f;

	return Float3(
		cosf(radian) * range,
		sinf(radian) * range,
		0.0f
	);
}
bool IsHit(
	Float3 position1,
	Float3 position2,
	float range
)
{
	if (
		position1.x - range < position2.x &&
		position1.x + range > position2.x &&
		position1.y - range < position2.y &&
		position1.y + range > position2.y)
	{
		//あたった
		return true;
	}
	//あたってない
	return false;

}
int MAIN()
{
	Window::SetSize(1000, 600);
	Mode mode = Title; //最初はタイトル画面にする

	Camera camera;
	camera.color = Float4(0.0f, 0.0f, 0.0f, 1.0f);

	Sprite Chest[3]; //宝の画像

	bool bossmuki = false;//false下　ture 上
	Chest[0].Load(L"宝1.png");
	Chest[1].Load(L"宝2.png");
	Chest[2].Load(L"宝3.png");
	for (int i = 0; i < 3; i++)
	{
		Chest[i].scale = 0.08f;
	}
	Sprite back[9];
	for (int i = 0; i < 9; i++)
	{
		back[i].Load(L"背景.png");
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			back[i + j * 3].position = Float2(-1000.0f + (i * 1000.0f), -600.0f + (j * 600.0f));
		}
	}
	Sprite HP[3];
	HP[0].Load(L"HP1.png");
	HP[1].Load(L"HP2.png");
	HP[2].Load(L"HP3.png");
	for (int i = 0; i < 3; i++)
	{
		HP[i].scale = 0.5f;
		HP[i].SetPivot(Float2(-0.883333f, 0.0f));
	}

	//true = まだ宝をとっていない　false = もう宝をとっている
	bool ChestFlag[3] = {};
	Sprite TITLE(L"タイトル画面.png");
	TITLE.scale = Float2(0.90f, 0.90f);
	TITLE.position.y = -25.0f;

	Sprite gameover(L"gameover.png");

	Sprite gameclear(L"gameclear.png");
	//宝箱の矢印の画像
	Sprite map[3];
	map[0].Load(L"矢印0.png");
	map[1].Load(L"矢印1.png");
	map[2].Load(L"矢印2.png");
	for (int i = 0; i < 3; i++)
	{
		map[i].scale = 0.1f;
	}
	int count = 0;

	float playerSpeed = 3.5f; //自機の速さ
	bool PlayerSpeed = false;

	Sprite player(L"player.png"); //自機の画像
	player.scale = 0.1f;
	player.SetPivot(Float2(0.0f, 0.25f));

	Sprite hidan(L"被弾.png");
	hidan.scale = 0.1f;


	const int enemyNum = 150; // 敵の数
	const float enemySpeed = 1.0f; //敵の速さ
	int enemylife[enemyNum] = { 3 }; //敵のライフ
	bool ENEMY[enemyNum] = { false };//敵の生死 false =　生　true = 死
	Sprite enemy(L"enemy.png"); //敵の画像
	enemy.scale = 0.1f;

	//ランダムなスピード
	float randspeed = 1.0f;

	//永遠に増える数
	int j = 0;

	Float3 enemyPosition[enemyNum]; //敵の座標
	//宝箱のスコア
	const float Score = 50.0f;

	const int bulletNum = 10; //弾の数
	const float bulletSpeed = 10.0f; //弾の速さ
	int bulletIndex = 0; //次に発射する弾の番号
	//弾の角度
	Float3 BulletRadian[bulletNum];
	Sprite bullet(L"弾2.png"); //弾の画像
	bullet.scale = 0.03f;
	Float3 bulletPosition[bulletNum]; // 弾の座標
	//次に発射するまでのカウント
	int bulletInterval = 0;
	int Rand = 0;
	int Randtime = 0;
	int playerangle = 0;//プレイヤーの向き　0 = 上　1 = 右　2 = 下　3 = 左
	bool playerAngle = false; //プレイヤーの向き２　false 左　　true 右
	int playerlife = 0;//プレイヤーのHp
	int lifetime = 0;//プレイヤーの無敵時間
	bool Life = false;//無敵かどうかfalse = 通常時　true = 無敵
	float bossRadian = 0.0f;
	bool Flag = false;
	Sprite lifeSprite[3];
	for (int i = 0; i < 3; i++)
	{
		lifeSprite[i].Load(L"player.png");
		lifeSprite[i].angles.z = 90.0f;
		lifeSprite[i].scale = 0.08f;
	}

	int score = 0; //スコア

	int enemyscore = 0; //スコア

	Sprite boss(L"ボス.png");
	boss.scale = 0.5f;
	Sprite bossposition(L"被弾.png");
	bossposition.scale = 3.0f;
	int bosstime = 0;
	bool BOSS = false;//true = ボスがいる false = ボスがいない
	const int bossbulletNum = 200; //弾の数
	const float bossbulletSpeed = 1.7f; //弾の速さ
	int bossbulletIndex = 0; //次に発射する弾の番号
	 //弾の角度
	float bossBulletRadian[bossbulletNum];
	Sprite bossbullet(L"弾1.png"); //弾の画像
	bossbullet.scale = 0.05f;
	Float3 bossbulletPosition[bossbulletNum]; // 弾の座標
	//次に発射するまでのカウント
	int bossbulletInterval = 0;
	int bosslife = 0;
	int maxlife = 0;
	while (Refresh())
	{
		camera.Update();
		Rand = rand();
		j++;
		if (j == 15)
		{
			randspeed = (float)(Rand % 4 + 5) / 4;
			j = 0;
			Randtime = Rand;
		}
		switch (mode)
		{
		case Title:
			//ゲーム開始
			camera.position = 0.0;
			TITLE.Draw();
			playerangle = 1;
			playerAngle = true;
			bossmuki = false;
			player.position = 0.0;
			enemyscore = 0;
			score = 0;
			playerlife = 3;
			lifetime = 0;
			Life = false;
			bosstime = 0;
			PlayerSpeed = false;
			boss.position = Float3(0.0f, 1500.0f, 0.0f);
			//敵の座標の初期化
			for (int i = 0; i < enemyNum; i++)
			{
				enemyPosition[i] = GetRandomPosition();
				enemylife[i] = 2;
				ENEMY[i] = false;
			}
			//宝フラグの初期化
			for (int i = 0; i < 3; i++)
			{
				ChestFlag[i] = false;
			}
			//弾の座標を初期化
			for (int i = 0; i < bulletNum; i++)
			{
				//はるか彼方に飛ばす
				bulletPosition[i].x = 99999.0f;
				BulletRadian[i] = 0.0;
			}
			//弾の座標を初期化
			for (int i = 0; i < bossbulletNum; i++)
			{
				//はるか彼方に飛ばす
				bossbulletPosition[i].x = 99999.0f;
				bossBulletRadian[i] = 0.0;
			}
			for (int i = 0; i < 3; i++)
			{
				map[i].position = Float2(0.0f, 0.0f);
			}
			if (Input::GetKeyDown(VK_SPACE))
			{
				mode = Game;
				bosslife = 100;
				maxlife = bosslife;
				BOSS = false;
			}
			if (Input::GetKey('0') && Input::GetKey('1'))
			{
				mode = Game;
				bosslife = 300;
				maxlife = bosslife;
				BOSS = true;
			}
			break;

		case Game:
			if (!PlayerSpeed)
			{
				playerSpeed = 3.5f;
			}
			else
			{
				playerSpeed = 2.85f;
			}

			for (int i = 0; i < 9; i++)
			{
				back[i].Draw();
			}
			for (int i = 0; i < 3; i++)
			{
				lifeSprite[i].position = Float2(camera.position.x + 450.0f - (i * 70), camera.position.y + 250.0f);
				if ((i + 1) * 40 == enemyscore && !ChestFlag[i])
				{
					Chest[i].position = Float2(Rand * rand() % 3000 - 1499, Rand * rand() % 1800 - 899);
					ChestFlag[i] = true;
				}
			}
			for (int i = 0; i < 3; i++)
			{
				HP[i].position = Float2(camera.position.x - 300.0f, camera.position.y + 250.0f);
			}
			if (!Life)
			{
				hidan.position.x = player.position.x;
				hidan.position.y = player.position.y;
			}
			else if (lifetime < 60)
			{
				hidan.Draw();
			}
			//宝箱との当たり判定
			for (int i = 0; i < 3; i++)
			{
				if (enemyscore >= Score * (i + 1))
				{
					if (ChestFlag[i] == true)
					{
						if (Chest[i].position.x >= player.position.x &&
							Chest[i].position.y - player.position.y >= Chest[i].position.x - player.position.x)
						{
							//北北東に宝がある時                                                              
							map[i].position = Float2(player.position.x + 50.0f, player.position.y + 42.0f);
							map[i].angles.z = -22.5;
						}
						else if (Chest[i].position.x >= player.position.x &&
							Chest[i].position.y - player.position.y <= Chest[i].position.x - player.position.x&&
							player.position.y < Chest[i].position.y)
						{
							//東北東に宝がある時
							map[i].position = Float2(player.position.x + 75.0f, player.position.y + 22.0f);
							map[i].angles.z = -67.5;
						}
						else if (Chest[i].position.x >= player.position.x &&
							Chest[i].position.y - player.position.y >= -Chest[i].position.x - -player.position.x &&
							player.position.y >= Chest[i].position.y)
						{
							//東南東に宝があるとき
							map[i].position = Float2(player.position.x + 75.0f, player.position.y + -22.0f);
							map[i].angles.z = -112.5;
						}
						else if (Chest[i].position.x >= player.position.x &&
							Chest[i].position.y - player.position.y <= -Chest[i].position.x - -player.position.x&&
							player.position.y >= Chest[i].position.y)
						{
							//南南東に宝があるとき
							map[i].position = Float2(player.position.x + 50.0f, player.position.y + -42.0f);
							map[i].angles.z = -157.5;
						}
						else if (Chest[i].position.x <= player.position.x &&
							Chest[i].position.y - player.position.y <= Chest[i].position.x - player.position.x &&
							player.position.y >= Chest[i].position.y)
						{
							//南南西に宝があるとき
							map[i].position = Float2(player.position.x + -50.0f, player.position.y + -42.0f);
							map[i].angles.z = -202.5;
						}
						else if (Chest[i].position.x <= player.position.x &&
							player.position.y >= Chest[i].position.y >= Chest[i].position.x - player.position.x &&
							player.position.y >= Chest[i].position.y)
						{
							//西南西に宝があるとき
							map[i].position = Float2(player.position.x + -75.0f, player.position.y + -2.0f);
							map[i].angles.z = -247.5;
						}
						else if (Chest[i].position.x <= player.position.x &&
							Chest[i].position.y - player.position.y <= -Chest[i].position.x - -player.position.x&&
							player.position.y <= Chest[i].position.y)
						{
							//西北西に宝があるとき
							map[i].position = Float2(player.position.x + -75.0f, player.position.y + 22.0f);
							map[i].angles.z = -292.5;
						}
						else if (Chest[i].position.x <= player.position.x &&
							Chest[i].position.y - player.position.y >= -Chest[i].position.x - -player.position.x&&
							player.position.y <= Chest[i].position.y)
						{
							//北北西に宝があるとき
							map[i].position = Float2(player.position.x + -50.0f, player.position.y + 42.0f);
							map[i].angles.z = -337.5;
						}
						map[i].Draw();
					}
					float hitRange = Chest[i].GetSize().x / 1.5f *
						Chest[i].scale.x;
					if (IsHit(Chest[i].position,
						player.position,
						hitRange
					) && ChestFlag[i])
					{
						score++;
						ChestFlag[i] = false;
					}
					if (!ChestFlag[i])
					{
						Chest[i].position = Float2(camera.position.x + 450.0f - (i * 70), camera.position.y + 180.0f);
					}
				}
			}
			for (int i = 0; i < 3; i++)
			{
				if ((i + 1) * Score <= enemyscore)
				{
					Chest[i].Draw();
				}
			}
			if (score >= 3)
			{
				BOSS = true;
			}
			if (BOSS)
			{
				//ボスのHPの表示
				for (int i = 0; i < 3; i++)
				{
					HP[i].Draw();
				}
				boss.Draw();
				if (HP[1].scale.x != bosslife * 0.5 && bosslife >= 0 && maxlife == 100 || HP[1].scale.x != bosslife * 0.5f / 3 && bosslife >= 0 && maxlife == 300)
				{
					HP[1].scale.x = (float)bosslife / maxlife * 0.5;
				}
				bossposition.position = Float3(boss.position.x, boss.position.y, 0.0f);
				bossposition.angles.z -= rand() % 50;
				if (bossmuki == false)
				{
					boss.position.y -= 1.5f;
				}
				else
				{
					boss.position.y += 1.5f;
				}
				if (boss.position.y == 300)
				{
					bossmuki = false;
				}
				if (boss.position.y == -300)
				{
					bossmuki = true;
				}
				//雑魚敵を飛ばす
				for (int i = 55; i < enemyNum - 55; i++)
				{
					if (i <= 55 || i >= 145)
					{
						ENEMY[i] = true;
						enemyPosition[i] = Float3(10000000.0f, 0.0f, 0.0f);
					}
				}
				bossbulletInterval++;
				if (bossbulletInterval > 4 || bossbulletInterval > 2 && maxlife == 300)
				{
					bossbulletInterval = 0;
					//弾をプレイヤーの座標と角度に合わせる
					bossbulletPosition[bossbulletIndex] = boss.position;
					if (bossbulletIndex % 2 == 0)
					{
						bossRadian = bossposition.angles.z / 180.0f * XM_PI;
					}
					else
					{
						bossRadian = (bossposition.angles.z + 180.0f) / 180.0f * XM_PI;
					}
					bossBulletRadian[bossbulletIndex] = bossRadian;
					bossbulletIndex++;
					if (bossbulletIndex >= bossbulletNum)
					{
						bossbulletIndex = 0;
					}
				}
				for (int i = 0; i < bossbulletNum; i++)
				{
					//弾の移動
					bossbulletPosition[i] += Float3(
						cosf(bossBulletRadian[i]),
						sinf(bossBulletRadian[i]),
						0.0
					) * bossbulletSpeed * 2;
					bossbullet.position = bossbulletPosition[i];
					//弾の描画
					bossbullet.Draw();
				}
				if (bosslife <= 0)
				{
					bossposition.Draw();
					bosstime++;
					for (int i = 0; i < bossbulletNum; i++)
					{
						//弾の移動
						bossbulletPosition[i] = Float3(100000.0f, 0.0f, 0.0f
						);
					}
					for (int i = 0; i < enemyNum; i++)
					{
						ENEMY[i] = true;
						enemyPosition[i] = Float3(10000000.0f, 0.0f, 0.0f);
					}
					if (bosstime >= 90)
					{
						mode = Gameclear;
					}
				}
			}

			//自機の移動の処理
			if (lifetime > 50 || lifetime == 0)
			{
				if (Input::GetKey('D') || Input::GetKey(VK_RIGHT))
				{
					if (player.position.x < 1495)
					{
						player.position.x += playerSpeed;
					}
					if (!Input::GetKey('A') && !Input::GetKey(VK_LEFT))
					{
						if (!Input::GetKey(VK_SPACE))
						{
							playerangle = 1;
							playerAngle = true;
						}
					}
					if (Input::GetKey('W') || Input::GetKey(VK_UP) || Input::GetKey('S') || Input::GetKey(VK_DOWN))
					{
						PlayerSpeed = true;
					}
					else
					{
						PlayerSpeed = false;
					}
				}

				if (Input::GetKey('A') || Input::GetKey(VK_LEFT))
				{
					if (player.position.x > -1495)
					{
						player.position.x -= playerSpeed;
					}
					if (!Input::GetKey('D') && !Input::GetKey(VK_RIGHT))
					{
						if (!Input::GetKey(VK_SPACE))
						{
							playerangle = 3;
							playerAngle = false;
						}
					}
					if (Input::GetKey('W') || Input::GetKey(VK_UP) || Input::GetKey('S') || Input::GetKey(VK_DOWN))
					{
						PlayerSpeed = true;
					}
					else
					{
						PlayerSpeed = false;
					}
				}

				if (Input::GetKey('W') || Input::GetKey(VK_UP))
				{
					if (player.position.y < 895)
					{
						player.position.y += playerSpeed;
					}
					if (!Input::GetKey('D') && !Input::GetKey(VK_RIGHT) &&
						!Input::GetKey('A') && !Input::GetKey(VK_LEFT) &&
						!Input::GetKey('S') && !Input::GetKey(VK_DOWN))
					{
						PlayerSpeed = false;
						if (!Input::GetKey(VK_SPACE))
						{
							playerangle = 2;
						}
					}
				}

				if (Input::GetKey('S') || Input::GetKey(VK_DOWN))
				{
					if (player.position.y > -895)
					{
						player.position.y -= playerSpeed;
					}
					if (!Input::GetKey('D') && !Input::GetKey(VK_RIGHT) &&
						!Input::GetKey('A') && !Input::GetKey(VK_LEFT) &&
						!Input::GetKey('W') && !Input::GetKey(VK_UP))
					{
						PlayerSpeed = false;
						if (!Input::GetKey(VK_SPACE))
						{
							playerangle = 0;
						}
					}
				}
			}

			if (playerAngle)
			{
				player.angles.x = 180;
			}
			else
			{
				player.angles.x = 0;
			}

			if (playerangle == 1)
			{
				player.angles.z = 90;
			}

			if (playerangle == 0)
			{
				if (playerAngle)
				{
					player.angles.z = 0;
				}
				else
				{
					player.angles.z = 180;
				}
			}

			if (playerangle == 3)
			{
				player.angles.z = 90;
			}

			if (playerangle == 2)
			{
				if (playerAngle)
				{
					player.angles.z = 180;
				}
				else
				{
					player.angles.z = 0;
				}
			}

			//自機の描画
			if (!Life || lifetime >= 60 && lifetime <= 70 || lifetime >= 80 && lifetime <= 90 || lifetime >= 100 && lifetime <= 110)
			{
				player.Draw();
			}
			for (int i = 0; i < 3; i++)
			{
				if (playerlife - i - 1 >= 0)
				{
					lifeSprite[playerlife - i - 1].Draw();
				}
			}
			//カメラを自機に追従させる
			if (player.position.x < 1000 &&
				player.position.x > -1000)
			{
				camera.position.x = player.position.x;
			}
			if (player.position.y < 600 &&
				player.position.y > -600)
			{
				camera.position.y = player.position.y;
			}

			bulletInterval++;
			if (bulletInterval > 10)
			{
				bulletInterval = 0;
				//弾をプレイヤーの座標と角度に合わせる
				bulletPosition[bulletIndex] =
					player.position;
				if (playerangle == 0)
				{
					BulletRadian[bulletIndex] = Float3(0.0f, -1.0f, 0.0f);
				}
				if (playerangle == 1)
				{
					BulletRadian[bulletIndex] = Float3(1.0f, 0.0f, 0.0f);
				}
				if (playerangle == 2)
				{
					BulletRadian[bulletIndex] = Float3(0.0f, 1.0f, 0.0f);
				}
				if (playerangle == 3)
				{
					BulletRadian[bulletIndex] = Float3(-1.0f, 0.0f, 0.0f);
				}
				bulletIndex++;
				if (bulletIndex >= bulletNum)
					bulletIndex = 0;

			}
			for (int i = 0; i < enemyNum; i++)
			{
				float enemyRadian = atan2f(
					player.position.y - enemyPosition[i].y,
					player.position.x - enemyPosition[i].x
				);
				//もし画面外にいれば湧きなおす
				if (enemyPosition[i].x >= 1505 && ENEMY[i] == false ||
					enemyPosition[i].x <= -1505 && ENEMY[i] == false ||
					enemyPosition[i].y >= 905 && ENEMY[i] == false ||
					enemyPosition[i].y <= -905 && ENEMY[i] == false)
				{
					//画面外に湧かせる
					while (count != 1)
					{
						Flag = false;
						enemyPosition[i] =
							Float3(Rand % 4000 - 2000.0f, rand() % 2700 - 1350.0f, 0.0f);
						for (int g = 0; g < count + 1; g++)//1
						{
							if (enemyPosition[i].x <= camera.position.x + (Window::GetSize().x / 2) + 600.0f&&
								enemyPosition[i].x >= camera.position.x - (Window::GetSize().x / 2) - 600.0f &&
								enemyPosition[i].y <= camera.position.y + (Window::GetSize().y / 2) + 450.0f &&
								enemyPosition[i].y >= camera.position.y - (Window::GetSize().y / 2) - 450.0f)
							{
								Flag = true;
								break;
							}
							g = 0;
						}
						if (!Flag)
						{
							count++;
						}
					}
					count = 0;
				}
				//敵を動かす処理
				if (i <= 0)
				{
					enemyPosition[i] += Float3(
						cosf(enemyRadian),
						sinf(enemyRadian),
						0.0
					) * enemySpeed;
				}
				else if (i <= 135)
				{
					if (enemyPosition[i].x <= player.position.x + (Window::GetSize().x / 4) &&
						enemyPosition[i].x >= player.position.x - (Window::GetSize().x / 4) &&
						enemyPosition[i].y <= player.position.y + (Window::GetSize().y / 4) &&
						enemyPosition[i].y >= player.position.y - (Window::GetSize().y / 4))
					{
						enemyPosition[i] += Float3(
							cosf(enemyRadian),
							sinf(enemyRadian),
							0.0
						) * 1.5f;
					}
					else
					{
						enemyPosition[i] += Float3(
							cosf(Randtime / i),
							sinf(Randtime / i),
							0.0
						) * enemySpeed * 2.0f;
					}
				}
				else if (i <= 140)
				{
					enemyPosition[i] += Float3(
						cosf(enemyRadian),
						sinf(enemyRadian),
						0.0
					) * randspeed * ((i % 4 + 1) / 4.0f);
				}
				else
				{
					enemyPosition[i] += Float3(
						cosf(enemyRadian),
						sinf(enemyRadian),
						0.0
					) * 1.5f;
				}
				enemy.position = enemyPosition[i];
				float hitRange = 20.0f;

				//弾と当たり判定
				for (int j = 0; j < bulletNum; j++)
				{
					if (IsHit(boss.position,
						bulletPosition[j],
						125.0f
					) && BOSS)
					{
						bulletPosition[j] = Float2(10000.0f, 0.0f);
						bosslife--;
					}
					if (IsHit(enemyPosition[i],
						bulletPosition[j],
						hitRange
					))
					{
						if (enemylife[i] > 0)
						{
							enemylife[i]--;
							//画面外に湧かせる
							while (count != 1)
							{
								Flag = false;
								enemyPosition[i] =
									Float3(Rand % 3000 - 1500.0f, rand() % 1800 - 900.0f, 0.0f);
								for (int g = 0; g < count + 1; g++)//1
								{
									if (enemyPosition[i].x <= camera.position.x + 500.0f + 600.0f&&
										enemyPosition[i].x >= camera.position.x - 500.0f - 600.0f &&
										enemyPosition[i].y <= camera.position.y + 300.0f + 450.0f &&
										enemyPosition[i].y >= camera.position.y - 300.0f - 450.0f)
									{
										Flag = true;
										break;
									}
									g = 0;
								}
								if (!Flag)
								{
									count++;
								}
							}
						}
						else
						{
							ENEMY[i] = true;
							enemyPosition[i] = Float3(10000000.0f, 0.0f, 0.0f);
						}
						bulletPosition[j] = Float2(10000.0f, 0.0f);
						count = 0;
						enemyscore++;
					}
				}
				//自機との当たり判定
				if (IsHit(
					enemyPosition[i],
					player.position,
					hitRange
				) && !Life)
				{
					playerlife--;
					Life = true;
				}
				//敵を描画
				enemy.Draw();
			}
			if (IsHit(boss.position,
				player.position,
				125.0f
			) && BOSS && !Life)
			{
				playerlife--;
				Life = true;
			}
			for (int i = 0; i < bossbulletNum; i++)
			{
				float hitRange = player.GetSize().x / 3.5f * player.scale.x;
				enemy.scale.x;
				if (IsHit(player.position,
					bossbulletPosition[i],
					hitRange
				) && !Life)
				{
					playerlife--;
					Life = true;
				}
			}
			//無敵時間の処理
			if (Life)
			{
				lifetime++;
				if (lifetime >= 120)
				{
					lifetime = 0;
					Life = false;
				}
			}
			if (playerlife <= 0 && lifetime >= 60)
			{
				camera.position = 0.0;
				mode = Gameover;
			}
			for (int i = 0; i < bulletNum; i++)
			{
				//弾の移動
				bulletPosition[i] += Float3(BulletRadian[i].x, BulletRadian[i].y, 0.0f)
					* bulletSpeed;
				bullet.position = bulletPosition[i];
				//弾の描画
				bullet.Draw();
			}
			break;
		case Gameover:
			camera.position = 0.0;
			gameover.Draw();
			if (Input::GetKeyDown(VK_SPACE))
			{
				mode = Title;
			}
			break;
		case Gameclear:
			camera.position = 0.0;
			gameclear.Draw();
			if (Input::GetKeyDown(VK_SPACE))
			{
				mode = Title;
			}
			break;
		}
	}
	return 0;
}
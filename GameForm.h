#pragma once

#include <time.h>
#include <stdlib.h>

namespace SnakeGame {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;
	using namespace System::Media;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class GameForm : public System::Windows::Forms::Form
	{
	public:
		GameForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GameForm()
		{
			if (components)
			{
				delete components;
			}
		}

		virtual void OnPaint(System::Object^ obj, PaintEventArgs^ e) override
		{
			Graphics^ g = e->Graphics;

			Brush^ fruitBrush = gcnew SolidBrush(Color::Red);
			g->FillRectangle(fruitBrush, fruitPosition.X, fruitPosition.Y, blockSize, blockSize);
			Brush^ snakebodyBrush = gcnew SolidBrush(Color::Green);
			Brush^ snakeheadBrush = gcnew SolidBrush(Color::DarkGreen);
			for (int i = 0; i < snake->Count; i++)
			{
				if (i == 0) 
					g->FillRectangle(snakeheadBrush, snake[i].X, snake[i].Y, blockSize, blockSize);
				else 
					g->FillRectangle(snakebodyBrush, snake[i].X, snake[i].Y, blockSize, blockSize);
			}
		}

		void ResetGame()
		{
			snake->Clear();
			snake->Add(Point(200, 200));
			moveX = moveY = nextMoveX = nextMoveY = 0;
			up = down = left = right = false;
			inputProcessed = true;
			PlaceFruit();
			timer->Start();
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		List<Point>^ snake;
		Point fruitPosition;
		const int blockSize = 40;
		Timer^ timer;
		int moveX = 0, moveY = 0;
		int nextMoveX = 0, nextMoveY = 0;
		bool up = false, down = false, left = false, right = false;
		bool inputProcessed = true;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// GameForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(400, 400);
			this->Text = L"SnakeGame";
			this->BackColor = Color::Black;
			this->ResumeLayout(false);

			snake = gcnew List<Point>();
			snake->Add(Point(200, 200));

			srand(time(NULL));
			PlaceFruit();

			timer = gcnew Timer();
			timer->Interval = 140;
			timer->Tick += gcnew EventHandler(this, &GameForm::OnTimerTick);
			timer->Start();

			this->Paint += gcnew PaintEventHandler(this, &GameForm::OnPaint);
			this->KeyDown += gcnew KeyEventHandler(this, &GameForm::OnKeyDown);
		}
#pragma endregion
		void PlaceFruit()
		{
			int maxX = this->ClientSize.Width / blockSize;
			int maxY = this->ClientSize.Height / blockSize;

			do  
			{
				fruitPosition = Point(rand() % maxX * blockSize, rand() % maxY * blockSize);
				
			} 
			while (snake->Contains(fruitPosition));
		}

		void PlayEatSound() 
		{
			SoundPlayer^ player = gcnew SoundPlayer("eat.wav");
			player->Play(); 
		}

		void PlayGameOverSound() 
		{
			SoundPlayer^ player = gcnew SoundPlayer("game_over.wav");
			player->PlaySync(); 
		}

		void OnTimerTick(Object^ obj, EventArgs^ e)
		{

			if (nextMoveX != 0 || nextMoveY != 0)
			{
				moveX = nextMoveX;
				moveY = nextMoveY;
				nextMoveX = nextMoveY = 0;

				up = (moveY == -blockSize);
				down = (moveY == blockSize);
				left = (moveX == -blockSize);
				right = (moveX == blockSize);
			}

			MoveSnake();
			inputProcessed = true;

			if (snake[0].X < 0 || snake[0].Y < 0 || snake[0].X > ClientSize.Width || snake[0].Y > ClientSize.Height)
			{
				timer->Stop();
				PlayGameOverSound();
				if (MessageBox::Show("Game Over. Restart?", "Game Over", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes) 
				{
					ResetGame();
				}
				else
				{
					this->Close();
				}
				return;
			}

			if (snake->Count >= 4)
			{
				for (int i = 1; i < snake->Count; i++)
				{
					if (snake[0] == snake[i])
					{
						timer->Stop();
						PlayGameOverSound();
						if (MessageBox::Show("Game Over. Restart?", "Game Over", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes) 
						{
							ResetGame();
						}
						else
						{
							this->Close();
						}
						return;
					}
				}
			}

			if (snake[0] == fruitPosition)
			{
				PlayEatSound();
				GrowthSnake();
				PlaceFruit();
			}

			this->Invalidate();
		}

		void MoveSnake()
		{
			Point newHead = snake[0];

			newHead.X += moveX;
			newHead.Y += moveY;
			snake->Insert(0, newHead);
			snake->RemoveAt(snake->Count - 1);
		}

		void GrowthSnake()
		{
			Point newHead = snake[0];

			newHead.X += moveX;
			newHead.Y += moveY;
			snake->Insert(0, newHead);
		}

		void OnKeyDown(Object^ obj, KeyEventArgs^ e)
		{
			if (!inputProcessed)
			{
				switch (e->KeyCode)
				{
				case Keys::Up:
					if (!down)
					{
						nextMoveX = 0;
						nextMoveY = -blockSize;
					}
					break;
				case Keys::Down:
					if (!up)
					{
						nextMoveX = 0;
						nextMoveY = blockSize;
					}
					break;
				case Keys::Right:
					if (!left)
					{
						nextMoveX = blockSize;
						nextMoveY = 0;
					}
					break;
				case Keys::Left:
					if (!right)
					{
						nextMoveX = -blockSize;
						nextMoveY = 0;
					}
					break;
				}
			}
			else
			{
				switch (e->KeyCode)
				{
				case Keys::Up:
					if (!down)
					{
						moveX = 0;
						moveY = -blockSize;
						up = true;
						left = right = down = false;
					}
					break;
				case Keys::Down:
					if (!up)
					{
						moveX = 0;
						moveY = blockSize;
						down = true;
						left = right = up = false;
					}
					break;
				case Keys::Right:
					if (!left)
					{
						moveX = blockSize;
						moveY = 0;
						right = true;
						left = up = down = false;
					}
					break;
				case Keys::Left:
					if (!right)
					{
						moveX = -blockSize;
						moveY = 0;
						left = true;
						up = right = down = false;
					}
					break;
				}
				inputProcessed = false;
			}
		}
	};
}

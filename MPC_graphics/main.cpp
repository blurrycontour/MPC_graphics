#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

//#define SAVE_IMG


int dispRatio = 15;
int Lx;
int Ly;
int Lz;
float sigma;
int N;
float radius = 1.0;
Color particleColor = Color(255, 255, 255, 255);	//Solvent particle color
Color particleColor_n = Color(20, 20, 250, 255);	//Monomer color
Color outlineColor_n = Color(255, 255, 255, 255);	//Outline Color

int main(int argc, char** argv)
{
	if (argc > 1)
		if (stoi(argv[1]) != 0)
			dispRatio = stoi(argv[1]);

	char* anim_path;
	if (argc > 2)
		anim_path = argv[2];
	else
		anim_path = "../../anim.txt";

	fstream anim_file;
	anim_file.open(anim_path, ios::in);
	if (!anim_file.is_open())
	{
		cout << "Can't open file !" << endl;
		return -1;
	}
	
// Required Constants...	
	string _dat;
	anim_file >> _dat;
	int CYL = stoi(_dat);
	anim_file >> _dat;
	Lx = stoi(_dat);
	anim_file >> _dat;
	Ly = stoi(_dat);
	anim_file >> _dat;
	Lz = stoi(_dat); 
	anim_file >> _dat;
	sigma = stof(_dat); 
	anim_file >> _dat;
	N = stoi(_dat); 
	

	float radius_n = sigma*dispRatio;

	ContextSettings settings;
	settings.antialiasingLevel = 0;
	RenderWindow renderWindow(VideoMode(dispRatio*Lx, dispRatio*Ly), "MPC");
	renderWindow.setPosition(Vector2i(20, 50));
	renderWindow.setFramerateLimit(60);
	renderWindow.requestFocus();

	CircleShape SolventParticles;
	SolventParticles.setRadius(radius);
	SolventParticles.setFillColor(particleColor);

	CircleShape* MonomerParticles = new CircleShape[N];
	for (int i = 0; i < N; i++)
	{
		if (i==0)
			MonomerParticles[i].setFillColor(Color(255,50,50,255));
		else if (i == N-1)
			MonomerParticles[i].setFillColor(Color(50, 255, 50, 255));
		else
			MonomerParticles[i].setFillColor(particleColor_n);

		MonomerParticles[i].setRadius(radius_n);
		MonomerParticles[i].setOutlineThickness(-radius_n*0.1);
		MonomerParticles[i].setOutlineColor(outlineColor_n);
		MonomerParticles[i].setOrigin(radius_n, radius_n);
	}
	
	CircleShape Boundary;
	Boundary.setRadius(dispRatio * Lz / 2);
	Boundary.setFillColor(Color(10, 10, 10, 255));
	Boundary.setOutlineColor(Color(255, 255, 255, 255));
	Boundary.setOutlineThickness(1);
	Boundary.setOrigin(dispRatio * Lz / 2, dispRatio * Lz / 2);
	


#ifdef SAVE_IMG
	const int anim_size = 2000;
	const int anim_start = 0;
	Vector2u windowSize = renderWindow.getSize();
	Texture texture[anim_size];
	for (int i = 0; i < anim_size; i++)
		texture[i].create(windowSize.x, windowSize.y);

	char outfile[20] = { ' ' };
#endif

	int display_mode = 3;
	int iter = -1;
	string point[3] = { " " };
	float particle[3] = { 0.0 };
	

	while (!anim_file.eof())
	{
		iter++;
		Event event;
		while (renderWindow.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				renderWindow.close();
				return 0;
			}
			else if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case Keyboard::Num1:display_mode = 1; renderWindow.create(VideoMode(dispRatio*Ly, dispRatio*Lz), "MPC"); break;
				case Keyboard::Num2:display_mode = 2; renderWindow.create(VideoMode(dispRatio*Lz, dispRatio*Lx), "MPC"); break;
				case Keyboard::Num3:display_mode = 3; renderWindow.create(VideoMode(dispRatio*Lx, dispRatio*Ly), "MPC"); break;
				default: break;
				}
			}
		}

		//Draw the particles
		renderWindow.clear();
		if ((CYL == 1) && (display_mode == 1))
		{
			Boundary.setPosition(dispRatio*Lz / 2, dispRatio * Lz / 2);
			renderWindow.draw(Boundary);
		}
		for (int i = 0; i < N; i++)
		{
			anim_file >> point[0] >> point[1] >> point[2];
			particle[0] = stof(point[0]);
			particle[1] = stof(point[1]);
			particle[2] = stof(point[2]);
			switch (display_mode)
			{
			case 1:
				MonomerParticles[i].setPosition(particle[1] * dispRatio, particle[2] * dispRatio);
				break;
			case 2:
				MonomerParticles[i].setPosition(particle[2] * dispRatio, particle[0] * dispRatio);
				break;
			case 3:
				MonomerParticles[i].setPosition(particle[0] * dispRatio, particle[1] * dispRatio);
				break;
			default:
				MonomerParticles[i].setPosition(particle[1] * dispRatio, particle[2] * dispRatio); 
				break;
			}
			renderWindow.draw(MonomerParticles[i]);
		}
		
		renderWindow.display();
		

#ifdef SAVE_IMG
		if (iter < anim_size + anim_start && iter >= anim_start)
			texture[iter - 0].update(renderWindow);
		if (iter == anim_size + anim_start)
		{
			cout << "Saving captured images ... \n\n";
			for (int img = 0; img < anim_size; img++)
			{
				sprintf(outfile, "anim/shot_%d.png", img + 1);
				texture[img].copyToImage().saveToFile(outfile);
			}
			cout << "Saving complete !!! \n\n";
		}
#endif

	}



	/*RectangleShape lineX(Vector2f(HEIGHT, 1));
	lineX.setPosition(20, 0);
	lineX.rotate(90);*/

	anim_file.close();
	//getchar();

	return 0;
}
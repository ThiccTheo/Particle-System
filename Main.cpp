#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

class Particle
{
public:
	sf::Vector2f mesh[4];
	sf::RectangleShape body;
	int lifetime;
	static std::vector<Particle> particles;
	static const int maxLifetime;
	float angle;
	int alpha;
	sf::Vector2f direction;
	static sf::Color currentColor;

	Particle(const sf::Vector2f position, const sf::Vector2f direction)
	{
		alpha = 255;
		angle = static_cast<float>(rand() % 361);
		body.setSize(sf::Vector2f(4.f, 4.f));
		body.setOrigin(2.f, 2.f);
		body.setPosition(position);
		lifetime = maxLifetime;
		this->direction = direction;
	}

	static void draw(sf::RenderWindow* window)
	{
		sf::VertexArray vertexArray;
		vertexArray.setPrimitiveType(sf::Quads);
		vertexArray.resize(particles.size() * 4);

		int vertexPtr{ 0 };

		for (Particle& particle : particles)
		{
			const sf::FloatRect& bodyBounds{ particle.body.getGlobalBounds() };
			particle.mesh[0] = sf::Vector2f(bodyBounds.left, bodyBounds.top);
			particle.mesh[1] = sf::Vector2f(bodyBounds.left + bodyBounds.width, bodyBounds.top);
			particle.mesh[2] = sf::Vector2f(bodyBounds.left + bodyBounds.width, bodyBounds.top + bodyBounds.height);
			particle.mesh[3] = sf::Vector2f(bodyBounds.left, bodyBounds.top + bodyBounds.height);

			sf::Transform transform;
			transform.rotate(particle.angle);

			sf::Vertex* currentQuad{ &vertexArray[vertexPtr] };

			for (int i{ 0 }; i < 4; i++)
			{
				currentQuad[i] = transform.transformPoint(particle.mesh[i]);
				currentQuad[i].color = currentColor;
				currentQuad[i].color.a = particle.alpha;
			}

			vertexPtr += 4;
		}

		window->draw(vertexArray);
	}

	static void update(sf::RenderWindow* window)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2f direction{ sf::Mouse::getPosition(*window).x - 100.f, sf::Mouse::getPosition(*window).y - 100.f };
			float magnitude{ sqrt(direction.x * direction.x + direction.y * direction.y) };
			sf::Vector2f directionNormalized{ direction.x / magnitude, direction.y / magnitude };

			particles.emplace_back(sf::Vector2f(100.f, 100.f), directionNormalized);
		}

		for (Particle& particle : particles)
		{
			particle.lifetime--;
			particle.alpha = static_cast<int>(std::clamp(round(particle.lifetime / 255.f), 0.f, 255.f));
			
			particle.body.move(particle.direction.x * 0.1f, particle.direction.y * 0.1f);

			particle.body.setSize(sf::Vector2f(std::clamp(particle.body.getSize().x + 0.1f, 4.f, 30.f), std::clamp(particle.body.getSize().y + 0.1f, 4.f, 30.f)));
		}

		particles.erase(std::remove_if(particles.begin(), particles.end(),
			[&, window](const Particle& particle)
			{
				return particle.lifetime <= 0 || !particle.body.getGlobalBounds().intersects(sf::FloatRect(window->getViewport(window->getDefaultView())));
			}), particles.end());
	}
};

std::vector<Particle> Particle::particles;
const int Particle::maxLifetime{ 4000 };
sf::Color Particle::currentColor;

int main()
{
	srand(static_cast<unsigned int>(time(NULL)));
	sf::RenderWindow window(sf::VideoMode(600, 600), "Particle System", sf::Style::Default);

	Particle::currentColor = sf::Color(255, 255, 255);

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::MouseButtonPressed:
					if (e.mouseButton.button == sf::Mouse::Right)
					{
						Particle::currentColor = sf::Color(rand() % 256, rand() % 256, rand() % 256);
					}
					break;
			}
		}

		Particle::update(&window);
		window.setView(window.getDefaultView());
		window.clear(sf::Color::Black);
		Particle::draw(&window);
		window.display();

	}

	return EXIT_SUCCESS;
}

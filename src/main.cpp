#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>

sf::Vector3f march(sf::Vector3f _current_location, float _march_distance, sf::Vector2i _pixel, sf::Vector3f _camera = sf::Vector3f(0, 0, -1))
{
    sf::Vector3f directionVector = sf::Vector3f((float)(_pixel.x - 500) / 1000.0f, (float)(_pixel.y - 500) / 1000.0f, 0) - _camera;
    directionVector /= sqrt(directionVector.x * directionVector.x + directionVector.y * directionVector.y + directionVector.z * directionVector.z);
    return _current_location + _march_distance * directionVector;
}

float getDistance(sf::Vector3f _coordinates)
{
    _coordinates.z -= 3.0f;
    float distanceFromPlane = _coordinates.y + 1.0f;
    float distanceFromSphere = sqrt(_coordinates.x * _coordinates.x + _coordinates.y * _coordinates.y + _coordinates.z * _coordinates.z) - 0.85f;
    sf::Vector3 pointOnSphere = _coordinates / (distanceFromSphere + 0.85f);
    float r = std::cos(std::asin(pointOnSphere.x) * 25.0f) + std::cos(std::asin(pointOnSphere.y) * 25.0f);
    distanceFromSphere -= r / 50.0f;
    return distanceFromPlane >= distanceFromSphere ? distanceFromSphere : distanceFromPlane;
}
float getDistanceShadow(sf::Vector3f _coordinates)
{
    _coordinates.z -= 3.0f;
    float distanceFromPlane = _coordinates.y + 1.0f;
    float distanceFromSphere = sqrt(_coordinates.x * _coordinates.x + _coordinates.y * _coordinates.y + _coordinates.z * _coordinates.z) - 0.85f;
    sf::Vector3 pointOnSphere = _coordinates / (distanceFromSphere + 0.85f);
    float r = std::cos(std::asin(pointOnSphere.x) * 25.0f) + std::cos(std::asin(pointOnSphere.y) * 25.0f);
    distanceFromSphere -= r / 50.0f;
    return distanceFromSphere;
}

sf::Vector3f getNormal(sf::Vector3f _coordinates)
{
    float d = getDistance(_coordinates);
    float x = d - getDistance(_coordinates - sf::Vector3f(0.001, 0.0, 0.0));
    float y = d - getDistance(_coordinates - sf::Vector3f(0.0, 0.001, 0.0));
    float z = d - getDistance(_coordinates - sf::Vector3f(0.0, 0.0, 0.001));
    float normalize = sqrt(x * x + y * y + z * z);
    return sf::Vector3f(x, y, z) / normalize;
}

int main()
{
    sf::Clock frameTime;
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML works!");
    sf::Image output;
    output.create(1000, 1000, sf::Color(255, 0, 255));
    sf::Texture texture;
    if (!texture.loadFromImage(output))
    {
        throw std::invalid_argument("Could not load texture");
    }
    sf::Sprite drawableOutput(texture);
    sf::Event event;
    sf::Vector3f lightVector(-3.0f / 5.0f, 4.0f / 5.0f, 0.0f);
    while (window.isOpen())
    {

        for (int i = 0; i < 1000; i++)
        {
            for (int j = 0; j < 1000; j++)
            {
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                    {
                        window.close();
                    }
                }
                sf::Vector3f currentLocation = sf::Vector3f(0, 0, -1);
                bool hit = false;
                float hitDistance = 0.0f;
                for (int iter = 0; iter < 100000; iter++)
                {
                    float marchDistance = getDistance(currentLocation);
                    hitDistance += marchDistance;
                    if (marchDistance < 0.0001f)
                    {
                        hit = true;
                        break;
                    }
                    else if (hitDistance > 15.0f)
                    {
                        break;
                    }
                    currentLocation = march(currentLocation, marchDistance, sf::Vector2i(i, j));
                }
                if (hit)
                {
                    sf::Vector3 normalVec = getNormal(currentLocation);
                    float intensity = normalVec.x * lightVector.x + normalVec.y * lightVector.y;
                    intensity = (1.0f + intensity) / 2.3f;
                    output.setPixel(i, 999 - j, sf::Color(250.0 * intensity, 250.0 * intensity, 250.0 * intensity));
                    hitDistance = 0.0;
                    hit = false;
                    if (currentLocation.y + 1.0f < 0.01f)
                    {
                        for (int iter = 0; iter < 100000; iter++)
                        {
                            float marchDistance = getDistanceShadow(currentLocation);
                            hitDistance += marchDistance;
                            if (marchDistance < 0.00001f)
                            {
                                hit = true;
                                break;
                            }
                            else if (hitDistance > 15.0f)
                            {
                                break;
                            }
                            currentLocation = march(currentLocation, marchDistance, sf::Vector2i(i, j), -1.0f * lightVector);
                        }
                        if (hit)
                        {
                            output.setPixel(i, 999 - j, sf::Color(75, 75, 75));
                        }
                    }
                }
                else
                {
                    output.setPixel(i, 999 - j, sf::Color(75, 75, 75));
                }
            }
            if (texture.loadFromImage(output))
            {
                window.clear();
                window.draw(drawableOutput);
                window.display();
            }
        }
    }
    return 0;
}
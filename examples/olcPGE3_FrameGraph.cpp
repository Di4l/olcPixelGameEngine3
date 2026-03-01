#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

constexpr int GRAPH_WIDTH = 512;
constexpr int GRAPH_HEIGHT = 480;

class Frame_Graph : public olc::PixelGameEngine
{
public:
	Frame_Graph() {}

private:
	std::vector<float> vSamples;
	size_t nSampleIndex = 0;

public:
	bool OnUserCreate() override
	{
		vSamples.resize(GRAPH_WIDTH, 0.0f);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		vSamples[nSampleIndex] = fElapsedTime;
		nSampleIndex = (nSampleIndex + 1) % GRAPH_WIDTH;

		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		for (size_t i = 0; i < vSamples.size(); i++)
		{
			const float fHeight = std::log10(vSamples[i] * 1000.0f + 1e-6f) * 100.0f;
			draw.Line({(float)i, GRAPH_HEIGHT}, {(float)i, GRAPH_HEIGHT - fHeight}, (i == nSampleIndex - 1) ? olc::Colour::BLACK : olc::Colour::WHITE);
		}

		for(auto i: {10.0f, 100.0f, 1000.0f, 10000.0f})
		{
			const float fHeight = std::log10(float(i) + 1e-6f) * 100.0f;
			draw.Line({0.0f, GRAPH_HEIGHT - fHeight}, {20.0f, GRAPH_HEIGHT - fHeight}, olc::Colour::RED);
			draw.String({22.0f, GRAPH_HEIGHT - fHeight - 5.0f}, std::to_string(int(i)) + "ms", olc::Colour::RED);
		}

		return true;
	}
};

int main()
{
	Frame_Graph demo;

	if (demo.Construct({GRAPH_WIDTH, GRAPH_HEIGHT}, {2, 2}))
	{
		demo.Start();
	}

	return 0;
}
/*
	olc::PixelGameEngine3 Example - Procedural Fish

    Original Author: dandistine

    A small pond of procedural fish.

    Based on:
    https://www.youtube.com/watch?v=qlfh_rv6khY

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

#include <numbers>

struct Segment {
    olc::vf2d position;
    float angle {0.0f};
    float size {10.0f};
    float distance {3.0f};
    olc::Pixel color {olc::Colour::WHITE};
};

float rand_float(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
}

float rand_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

class Fish {
    std::vector<Segment> segments;
    std::vector<olc::vf2d> head_points;
    float max_speed = 40.0f;
    olc::Pixel outline_color = olc::Colour::BLACK;
    olc::Pixel fin_color = olc::Pixel{143, 73, 31};
    olc::Pixel fin_tip_color = olc::Pixel{217, 127, 13, 170};

    // An overly complicated function to determine the segment size
    // This was found largely by playing with desmos until something looked right
    // "form" lerps between a simple and more complex shape to allow some variation
    float SegmentSize(int i, int total_segments, float scale, float form = 1.0f) {
        float x = i;
        const float sqrt_3 = std::sqrt(3.0f);

        // A basic shape
        float val_1 = scale - (scale * i / (total_segments - 1));

        // A shape with a slightly smaller head and tapering tail
        float f = scale - (scale * (x - 2) / (total_segments - 1));
        float g = 1 + (sqrt_3 + 1) * (std::pow(((x + 1) / total_segments) - 1, 3)) +sqrt_3 * std::pow(((x + 1) / total_segments) - 1, 2);
        float j = g * f * f / (0.5 * total_segments);
        float val_2 = j + (total_segments - x - 1) / scale;

        return std::lerp(val_1, val_2, form);
    }

    public:
    Fish(olc::vf2d position, int num_segments, float total_length, float form = .50f, float scale = 1.0f) {
        olc::vf2d next_position {10.0f, 0.0f};

        segments.resize(num_segments);
        // segment_angles.resize(num_segments, 0.0f);
        float size = 11;
        float distance = total_length / num_segments;

        for(int i = 0; i < num_segments; i++) {
            float s = size - (size * i / (num_segments - 1));
            segments[i].size = SegmentSize(i, num_segments, size, form) * 1.2 * scale;
            segments[i].position = position + i * next_position;
            segments[i].angle = 0.0f;
            segments[i].distance = distance;
        }

        // Calculate the head points for drawing the head of the fish
        const int head_points_count = 10;
        // Angles for the top and bottom of the head
        const float start_angle = -std::numbers::pi_v<float> / 2.0f;
        const float end_angle = std::numbers::pi_v<float> / 2.0f;
        const float head_size = segments[0].size;

        for(int i = 0; i < head_points_count; i++) {
            float angle = start_angle + (end_angle - start_angle) * i / (head_points_count - 1);
            head_points.emplace_back(std::cos(angle) * head_size * (1 + 0.3*(fabs(start_angle) - fabs(angle))), std::sin(angle) * head_size);
        }
        
    }

    // Sum up the "total curve" of the fish.  This is useful for the tail
    float TotalCurve() {
        float total_curve = 0.0f;
        for(int i = 0; i < segments.size() - 1; i++) {
            float a = segments[i].angle - segments[i + 1].angle;
            if(a > std::numbers::pi_v<float>) {
                a -= 2 * std::numbers::pi_v<float>;
            } else if(a < -std::numbers::pi_v<float>) {
                a += 2 * std::numbers::pi_v<float>;
            }
            total_curve += a;
        }
        return total_curve;
    }

    void Draw(olc::Draw& pge) {
        const auto world_transform = pge.GetWorldTransform();
        const auto origin = olc::vf2d{0.0f, 0.0f};
        const auto half_pi = std::numbers::pi_v<float> / 2.0f;

        DrawFin(pge, segments.size() * 0.18, 1.0f);
        DrawFin(pge, segments.size() * 0.7, 2.0f);

        {
            // Draw a half circle for the head
            const auto& head = segments[0];
            olc::tf2d transform;
            transform.translate(head.position);
            transform.rotate(head.angle);
            pge.SetWorldTransform(transform);
            
            pge.FilledPolygon(olc::Structure::Fan, head_points, head.color);

            for(int i = 0; i < head_points.size() - 1; i++) {
                pge.Line(head_points[i], head_points[i + 1], outline_color);
            }
        }

        pge.SetWorldTransform(world_transform);

        // Batch used for the outline of the fish
        auto line_batch = pge.CreateLineBatch();

        // Draw a bunch of quads from one segment to the next to form the body of the fish
        for(int i = 0; i < segments.size() - 1; i++) {
            const Segment& seg_a = segments[i];
            const Segment& seg_b = segments[i + 1];

            float x1, y1, x2, y2;
            x1 = seg_a.position.x + std::cos(seg_a.angle + half_pi) * seg_a.size;
            y1 = seg_a.position.y + std::sin(seg_a.angle + half_pi) * seg_a.size;
            x2 = seg_b.position.x + std::cos(seg_b.angle + half_pi) * seg_b.size;
            y2 = seg_b.position.y + std::sin(seg_b.angle + half_pi) * seg_b.size;
            
            float x3, y3, x4, y4;
            x3 = seg_a.position.x + std::cos(seg_a.angle - half_pi) * seg_a.size;
            y3 = seg_a.position.y + std::sin(seg_a.angle - half_pi) * seg_a.size;
            x4 = seg_b.position.x + std::cos(seg_b.angle - half_pi) * seg_b.size;
            y4 = seg_b.position.y + std::sin(seg_b.angle - half_pi) * seg_b.size;

            std::vector<olc::vf2d> quad_points = {
                olc::vf2d(x1, y1),
                olc::vf2d(x2, y2),
                olc::vf2d(x4, y4),
                olc::vf2d(x3, y3)
            };

            pge.FilledPolygon(olc::Structure::Fan, quad_points, seg_a.color);

            pge.Line(line_batch, olc::vf2d(x1, y1), olc::vf2d(x2, y2), outline_color);
            pge.Line(line_batch, olc::vf2d(x3, y3), olc::vf2d(x4, y4), outline_color);
        }

        pge.Batch(line_batch);

        DrawDorsalFin(pge, segments.size() * 0.07, segments.size() * 0.24, 2.0f);
        DrawDorsalFin(pge, segments.size() * 0.6, segments.size() * 0.17, 1.0f);
        DrawEyes(pge);
        DrawTail(pge);
    }

    // Draw a simple tail triangle
    void DrawTail(olc::Draw& pge) {
        const auto world_transform = pge.GetWorldTransform();
        const auto& tail_start = segments[segments.size() - 5];
        const auto& end = segments[segments.size() - 1];
        const auto half_pi = std::numbers::pi_v<float> / 2.0f;

        float total_curve = TotalCurve();

        std::vector<olc::vf2d> tail_points;

        olc::tf2d transform;
        transform.translate(end.position);
        transform.rotate(end.angle);

        olc::vf2d p1 = tail_start.position;
        p1.x += std::cos(half_pi) * tail_start.size;
        p1.y += std::sin(half_pi) * tail_start.size;
        olc::vf2d p2 = end.position;
        olc::vf2d p3 = transform.forward<float>(olc::vf2d{-10.0f, total_curve / 1.5f});

        pge.SetWorldTransform(world_transform);
        pge.FilledTriangle(p1, p2, p3, fin_color);
    }

    // Draw some fins on the body of the fish
    void DrawFin(olc::Draw& pge, int segment_index, float size) {
        const auto world_transform = pge.GetWorldTransform();
        const auto& segment = segments[segment_index];
        const auto& prev_segment = segments[segment_index - 1];
        const auto& quarter_pi = std::numbers::pi_v<float> / 4.0f;
        const auto& third_pi = std::numbers::pi_v<float> / 3.0f;

        olc::tf2d transform_right;
        olc::tf2d transform_left;
        
        olc::vf2d offset = {std::cos(third_pi) * segment.size, std::sin(third_pi) * segment.size};
        olc::vf2d offset2 = {std::cos(-third_pi) * segment.size, std::sin(-third_pi) * segment.size};
        
        transform_right.translate(segment.position);
        transform_right.rotate(segment.angle + third_pi);
        transform_left.translate(segment.position);
        transform_left.rotate(segment.angle - third_pi);
        
        
        float fin_length = segment.size;
        float fin_width = segment.size * 0.5f;
        olc::vf2d position = {0.0f, 0.0f};
        
        olc::vf2d right_pos = transform_right.forwardRound<float>(olc::vf2d{segment.size, 0.0f});
        olc::vf2d left_pos = transform_left.forwardRound<float>(olc::vf2d{segment.size, 0.0f});
        
        olc::tf2d transform {};
        transform.translate(right_pos);
        transform.rotate(prev_segment.angle - quarter_pi);
        pge.SetWorldTransform(transform);
        pge.FilledEllipse({0.0f, 0.0f}, fin_length * size, fin_width * size, fin_color);

        
        transform = {};
        transform.translate(left_pos);
        transform.rotate(prev_segment.angle + quarter_pi);
        pge.SetWorldTransform(transform);
        pge.FilledEllipse({0.0f, 0.0f}, fin_length * size, fin_width * size, fin_color);
        

        pge.SetWorldTransform(world_transform);
    }

    // Draw the eyes on the head of the fish
    void DrawEyes(olc::Draw& pge) {
        const auto world_transform = pge.GetWorldTransform();
        const auto& head = segments[0];
        olc::tf2d transform;
        transform.translate(head.position);
        transform.rotate(head.angle);
        pge.SetWorldTransform(transform);

        float eye_offset_x = 0;
        float eye_offset_y = head.size * 0.3f;
        float eye_size = head.size * 0.3f;

        // Left eye
        pge.FilledCircle(head_points[2] * 0.9f, eye_size, olc::Colour::BLACK);
        pge.FilledCircle(head_points[2] * 0.9f, eye_size * 0.5f, olc::Colour::WHITE);

        // Right eye
        pge.FilledCircle(head_points[7] * 0.9f, eye_size, olc::Colour::BLACK);
        pge.FilledCircle(head_points[7] * 0.9f, eye_size * 0.5f, olc::Colour::WHITE);

        pge.SetWorldTransform(world_transform);
    }

    // Draw a fin on the back of the fish
    void DrawDorsalFin(olc::Draw& pge, int fin_start, size_t length, float size) {
        const int fin_end = fin_start + length;
        const auto world_transform = pge.GetWorldTransform();
        std::vector<olc::vf2d> fin_points {length * 2};
        std::vector<olc::Pixel> point_colors;
        point_colors.resize(fin_points.size());

        auto line_batch = pge.CreateLineBatch();

        for(int i = 0; i < length; i++) {
            Segment& seg = segments[i + fin_start];
            olc::tf2d transform;
            transform.translate(seg.position);
            transform.rotate(seg.angle);
            pge.SetWorldTransform(transform);

            float a = 3.0f + (i * 0.5f);
            float b = 3.0f + (length - i) * 1;
            float fin_scale = size * -1.0f * std::min(a, b);

            const auto& center_pos = pge.GetWorldTransform().translate();
            const auto& end_pos = pge.GetWorldTransform().forwardRound<float>(olc::vf2d{1.0f * fin_scale, 0.0f});

            fin_points[2 * i] = center_pos;
            fin_points[2 * i + 1] = end_pos;
            point_colors[2 * i] = fin_color;
            point_colors[2 * i + 1] = fin_tip_color;

            pge.Line(line_batch,olc::vf2d{0.0f, 0.0f}, olc::vf2d{1.0f * fin_scale, 0.0f}, fin_color * 0.3f);
        }

        pge.SetWorldTransform(world_transform);

        // Border on the fin
        for(int i = 1; i < fin_points.size() - 3; i+=2) {
            pge.Line(line_batch, fin_points[i], fin_points[i + 2], fin_color * 0.5f);
        }
        pge.Line(line_batch, fin_points[0], fin_points[1], fin_color * 0.5f);
        pge.Line(line_batch, fin_points[fin_points.size() - 2], fin_points[fin_points.size() - 1], fin_color * 0.5f);


        pge.Batch(line_batch);
        pge.FilledPolygon(olc::Structure::Strip, fin_points, point_colors);
    }

    void Update(float fElapsedTime, olc::vf2d target) {
        auto& head = segments[0];
        olc::vf2d to_target = target - head.position;
        // Rotate the head towards the target
        float angle = std::atan2(to_target.y, to_target.x);
        float delta = angle - head.angle;

        // Wrap the angle delta to the range [-pi, pi]
        while (delta < -std::numbers::pi_v<float>) delta += 2.0f * std::numbers::pi_v<float>;
        while (delta > std::numbers::pi_v<float>) delta -= 2.0f * std::numbers::pi_v<float>;
        head.angle += fElapsedTime * delta;

        // Also need to constrain the overall head angle
        if(head.angle > std::numbers::pi_v<float>) {
            head.angle -= 2.0f * std::numbers::pi_v<float>;
        } else if(head.angle < -std::numbers::pi_v<float>) {
            head.angle += 2.0f * std::numbers::pi_v<float>;
        }

        // Move the head forward a little bit
        head.position += olc::vf2d(std::cos(head.angle), std::sin(head.angle)) * max_speed * fElapsedTime;
        
        // Move each segment towards the previous one
        for(int i = 1; i < segments.size(); i++) {
            olc::vf2d to_prev = segments[i - 1].position - segments[i].position;
            segments[i].angle = std::atan2(to_prev.y, to_prev.x);
            float distance = to_prev.mag();
            if(distance > 0.0f) {
                olc::vf2d move = to_prev.norm() * (distance - segments[i].distance);
                segments[i].position += move;
            }
        }
    }

    bool IsPointInFish(olc::vf2d point) const {
        for(const auto& seg : segments) {
            if((point - seg.position).mag() < seg.size) {
                return true;
            }
        }
        return false;
    }
};

class Example_Fish : public olc::PixelGameEngine
{
public:
	Example_Fish()
	{

	}

protected:
    Fish fish{olc::vf2d(128.0f, 120.0f), 40, 80.0f, 1.0f};

    std::vector<Fish> others;
    std::vector<olc::vf2d> targets;
    bool fullscreen = false;
public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
        // Create some other fish to swim around
        for(int i = 0; i < 10; i++) {
            olc::vf2d pos {rand_float(0.0f, ScreenSize().x), rand_float(0.0f, ScreenSize().y)};
            others.emplace_back(pos, rand_int(25, 40), rand_float(60.0f, 100.0f), rand_float(0.5f, 1.0f), rand_float(0.5f, 1.5f));
            targets.emplace_back(rand_float(0.0f, ScreenSize().x), rand_float(0.0f, ScreenSize().y));
        }
		// Nothing to do here, so return true
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
        olc::Pixel background_color{73, 220, 222};
        if(GetKeyboard().GetKey(olc::Key::ALT).bHeld && GetKeyboard().GetKey(olc::Key::ENTER).bPressed) {
            fullscreen = !fullscreen;
            ShowFullScreen(fullscreen);
        }

        // Clear screen to a background color
		draw.Clear(background_color);

        // Draw the targets for the other fish
        for(const auto& target : targets) {
            draw.FilledCircle(target, 2, olc::Colour::DARK_GREEN);
        }

        // Update and draw the other fish
        for(int i = 0; i < others.size(); i++) {
            others[i].Update(fElapsedTime, targets[i]);
            others[i].Draw(draw);
        }

        // If a fish overlaps with a target, move teh target somewhere else
        for(auto& target : targets) {
            if(fish.IsPointInFish(target)) {
                target = olc::vf2d(rand_float(0.0f, ScreenSize().x), rand_float(0.0f, ScreenSize().y));
            } else {
                for(const auto& other : others) {
                    if(other.IsPointInFish(target)) {
                        target = olc::vf2d(rand_float(0.0f, ScreenSize().x), rand_float(0.0f, ScreenSize().y));
                        break;
                    }
                }
            }
        }

        // Update and draw the main fish last so it's on top
        fish.Update(fElapsedTime, GetMouse().GetPosition());
        fish.Draw(draw);

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Fish demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}
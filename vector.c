
float dist(vertex a, vertex b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float distance_squared = dx * dx + dy * dy;
    
    // safety check:
    if (distance_squared < 0) {
        return 0;
    }
    
    return my_sqrt(distance_squared);
}

vertex midpoint(vertex a, vertex b) {
    // returns a vertex between a and b
    vertex mid;
    mid.x = (a.x + b.x) / 2;
    mid.y = (a.y + b.y) / 2;
    mid.width = (a.width + b.width) / 2;
    mid.opacity = (a.opacity + b.opacity) / 2;
    mid.hardness = (a.hardness + b.hardness) / 2;
    return mid;
}

vertex midpoint_with_noise(vertex a, vertex b, float displacement_factor) {
    // returns a vertex between a and b with added displacement
    vertex mid;
    mid.x = average_with_noise(a.x, b.x, displacement_factor);
    mid.y = average_with_noise(a.y, b.y, displacement_factor);
    mid.width = average_with_noise(a.width, b.width, displacement_factor);
    mid.opacity = average_with_noise(a.opacity, b.opacity, displacement_factor);
    mid.hardness = average_with_noise(a.hardness, b.hardness, displacement_factor);
    return mid;
}

void perpendicular_vector(vertex a, vertex b, float *dx, float *dy) {
    *dx = b.y - a.y;
    *dy = a.x - b.x;
    float length = my_sqrt((*dx) * (*dx) + (*dy) * (*dy));
    // avoid division by zero:
    if (length == 0) {
        *dx = 0;
        *dy = 0;
    } else {
        *dx /= length;
        *dy /= length;
    }
}

void apply_perpendicular_displacement(vertex* mid, vertex a, vertex b, float displacement) {
    float dx, dy;
    perpendicular_vector(a, b, &dx, &dy);
    mid->x += (int32_t)(dx * displacement);
    mid->y += (int32_t)(dy * displacement);
}

// int noisy_neighbour_distance(vertex a, vertex b, vertex c) {
//     int dist1 = dist(a, b);
//     int dist2 = dist(b, c);
//     int average = average_with_noise(dist1, dist2, 0.1);
//     return average;
// }

// vertex displacement_from_dist(int dist, float displacement_factor) {
//     vertex displacement;
//     displacement.x = gaussian() * dist * 0.1;
//     displacement.y = gaussian() * dist * 0.1;
//     return displacement;
// }

int average_with_noise(int a, int b, float displacement_factor) {
    int average = (a + b) / 2;
    int noise_range = average * displacement_factor;
    // noise range should not be <= 0:
    if (noise_range <= 0) noise_range = 1;
    int average_with_noise = average + lcg_rand() % noise_range - noise_range / 2;
    return average_with_noise;
}



void apply_displacement(vertex* a, vertex displacement) {
    // printf("calling apply_displacement\n");
    int disp_x = a->x + displacement.x;
    int disp_y = a->y + displacement.y;
    // a->x = 
    // a->y = a->y + displacement.y;
    // printf("x: %d\n", disp_x);
    // printf("y: %d\n", disp_y);
    a->x = disp_x;
    a->y = disp_y;
    // printf("finished apply_displacement\n");
    return;
}

void displace_vertex(vertex *a, float radius, float displacement_factor) {
    // float radius = dist(*a, *b);
    float softness = (float)(255 - a->hardness) / 255.0;

    vertex displacement;
    displacement.x = gaussian() * radius * displacement_factor * softness;
    displacement.y = gaussian() * radius * displacement_factor * softness;
    a->x = a->x + displacement.x;
    a->y = a->y + displacement.y;
}

// float hardness_to_softness(uint8_t hardness, float softness) {
//     float softness = (float)(255 - hardness) / 255.0;
//     return softness;
// }

void subdivide_poly(vertex *poly,
                    vertex *sub_poly,
                    int n_vertices,
                    int n_levels,
                    float displacement_factor) {
    // if this is the last level, copy the final polygon and return:
    if (n_levels == 0) {
        for (int i = 0; i < n_vertices; i++) {
            sub_poly[i] = poly[i];
        }
        return;
    }
    // otherwise create a new polygon and fill in intermediate vertices:
    else {
        int new_n_vertices = n_vertices * 2;
        vertex new_poly[new_n_vertices];

        for (int i = 0; i < n_vertices; i++) {
            vertex a = poly[i];
            // if a is the last vertex, b is the first vertex:
            vertex b = (i == n_vertices - 1) ? poly[0] : poly[i + 1];

            // displace vertex a:
            float edge_length = dist(a, b);
            // vertex disp_vertex = displacement_from_dist(edge_length, displacement_factor);
            // apply_displacement(&a, disp_vertex);
            displace_vertex(&a, edge_length, displacement_factor);

            new_poly[i * 2] = a;

            float softness_factor = (1 - (a.hardness + b.hardness) / 2.0 / 255.0) * displacement_factor;
            vertex mid = midpoint_with_noise(a, b, softness_factor);
            // vertex mid = midpoint(a, b);
            float displacement = displacement_factor * gaussian() * edge_length * softness_factor;
            apply_perpendicular_displacement(&mid, a, b, displacement);

            new_poly[i * 2 + 1] = mid;
        }

        // recursively subdivide the new polygon:
        subdivide_poly(new_poly, sub_poly, new_n_vertices, n_levels - 1, displacement_factor);
    }
}

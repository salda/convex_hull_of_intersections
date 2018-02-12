CFLAGS  = -g -Wall -O2

convex_hull_of_intersections:  convex_hull_of_intersections.cpp
	g++ $(CFLAGS) -o convex_hull_of_intersections.exe convex_hull_of_intersections.cpp Ouellet_hull.cpp

clean: 
	$(RM) convex_hull_of_intersections.exe *.o *~
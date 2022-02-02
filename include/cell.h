class cell
{

    public:
        cell();
        bool is_taken;
        bool is_border;             //is border or the board, never can be taken
        bool did_anything;          //true if cell fought, traveled or bred in current turn

        float red, green, blue;

        float strength;             //increases by 2% of enemies strength after won fight
        int max_age;                //is constant

        int age;
                                    //increases once every turn or twice while fighting
        int reproduction_prio;    //any
        int fight_prio;           //+ value
        int travel_prio;          //is ok

        float mutation_chance;        //0.0f - 1.0f

        void take_action(cell* neighbours[8]);
        void multiply(cell* partner, cell* target);
        void fight(cell* enemy);
        void travel(cell* target);

};


/*
#include "classes.h"
Simulate::Simulate()
{

}

void  Simulate::AddEvent(Event *e, int time)
{
    event_queue[time].push_back(e);
}


void Simulate::runSimulation()
{
    int sim_time = 0;
    while(sim_time < threshold)
    {
        sim_time = event_queue.begin() -> first;
        //vector<Event*> v = event_queue.begin() -> second;
        vector<Event*> ::iterator it;
        /////////////////////////// might need to debug this ////////////////////////////////////////
        for(it = event_queue[sim_time].begin(); it != event_queue[sim_time].end(); it++)
        {
            Event* e = *(it);
            if(e->event_id == ID_FOR_RECEIVE_BLOCK)
            {
                e->receive_block_event(sim_time);
            }
            else if(e->event_id == ID_FOR_BROADCASTING_BLOCK)
            {
                e->broadcast_block_event(sim_time);
            }
            else if(e->event_id == ID_FOR_GEN_TRANS)
            {
                e->gen_trans_event(sim_time);
            }
            else if(e->event_id == ID_FOR_RECEIVE_TRANS)
            {
                e->receive_trans_event(sim_time);
            }
            else if(e->event_id == ID_FOR_CHECK_AND_BROADCAST_BLOCK){
                e->check_and_broadcast_block_event(sim_time);
            }
        }

        event_queue.erase(event_queue.begin());
    }
}

*/
#include "classes.h"
Simulate::Simulate()
{

}

void static Simulate::AddEvent(Event *e, int time)
{
    event_queue[time].push_back(e);
}

void static Simulate::removeEvent(int event_id, int node_id, int time)
{
    for(int i=0;i<event_queue[time].size();i++)
    {
        if(event_queue[time][i]->event_id == event_id && event_queue[time][i]->node_id == node_id)
        {
            auto iterator = event_queue[time].begin() + i;
            event_queue[time].erase(iterator);
            break;
        }
    }
}

void Simulate::runSimulation()
{
    int sim_time = 0;
    while(sim_time < threshold)
    {
        sim_time = event_queue.begin() -> first;
        //vector<Event*> v = event_queue.begin() -> second;
        vector<Event*> ::iterator it;
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
        }

        event_queue.erase(event_queue.begin());
    }
}
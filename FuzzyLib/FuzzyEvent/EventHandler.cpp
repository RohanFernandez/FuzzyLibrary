#pragma once
#include "EventSystem.h"
#include "EventHandler.h"

namespace FuzzyLib
{
#pragma region Event Handler

	//Deletes all registered listeners in this EventHandler 
	//Deletes all lists of all types in unordered map
	void EventHandler::DeleteAllListeners()
	{
		for (auto l_pIterator = m_pUMapRegisteredListener->begin();
			l_pIterator != m_pUMapRegisteredListener->end(); l_pIterator++)
		{
			FuzzyList<RegisteredListener*>*& l_pCurrFuzzyList = static_cast<FuzzyList<RegisteredListener*>*>(l_pIterator->second);
			FuzzyList<RegisteredListener*> l_refCurrFuzzyList = *l_pCurrFuzzyList;

			int l_iCurrListCount = l_pCurrFuzzyList->GetCount();
			for (int l_iListindex = 0; l_iListindex < l_iCurrListCount; l_iListindex++)
			{
				delete l_refCurrFuzzyList[l_iListindex];
				l_refCurrFuzzyList[l_iListindex] = nullptr;
			}
			delete l_pCurrFuzzyList;
			l_pCurrFuzzyList = nullptr;
		}
		m_pUMapRegisteredListener->clear();
	}

	//Constructor to create a single event handler
	EventHandler::EventHandler()
		: m_pEventSystem{ EventSystem::GetInstance() }
	{
		m_pEventSystem->RegisterEventHandler(this);
		m_pUMapRegisteredListener = new std::unordered_map<std::type_index, FuzzyList<RegisteredListener*>*>();
	};

	EventHandler::~EventHandler()
	{
		DeleteAllListeners();
		m_pEventSystem->UnregisterEventHandler(this);

		delete m_pUMapRegisteredListener;
		m_pUMapRegisteredListener = nullptr;
	}

	//Get the unordered map's container count
	const int EventHandler::GetCount() const
	{
		return m_pUMapRegisteredListener->size();
	}

	//Fires the event, dispatches all the events
	void EventHandler::Fire(IEvent& a_IEvent)
	{
		const std::type_info& l_TypeID = typeid(a_IEvent);
		std::cout << "Event type:: '"<<l_TypeID.name() << "'\n";

		if (m_pUMapRegisteredListener->find(l_TypeID) == m_pUMapRegisteredListener->end())
		{
			return;
		}

		FuzzyList<RegisteredListener*>& l_reflstRegisteredListener = *(m_pUMapRegisteredListener->at(l_TypeID));

		int l_iListCount = l_reflstRegisteredListener.GetCount();
		for (int l_iRegisteredindex = 0; l_iRegisteredindex < l_iListCount; l_iRegisteredindex++)
		{
			if (&a_IEvent == l_reflstRegisteredListener[l_iRegisteredindex]->m_pIEvent
				|| &a_IEvent == nullptr)
			{
				l_reflstRegisteredListener[l_iRegisteredindex]->m_pEventListener->Dispatch(a_IEvent);
			}
		}

	}

	void EventHandler::DebugLog()
	{
		for (auto l_iterator = m_pUMapRegisteredListener->begin();
			l_iterator != m_pUMapRegisteredListener->end();
			l_iterator++)
		{
			std::cout<<l_iterator->first.name()<<"\n";
		}
	}

#pragma endregion Event Handler

#pragma region Registered Listener

	//Constructor for the Value of the unordered map
	//includes List of registered Listeners for the event of a particular type
	EventHandler::RegisteredListener::RegisteredListener(IEvent* a_pEventObject, IEventListener* a_pListenerObj)
	{
		m_pIEvent = a_pEventObject;
		m_pEventListener = a_pListenerObj;
	}

	//Destructor for the Value of the unordered map
	EventHandler::RegisteredListener::~RegisteredListener()
	{

	}

	//Returns true if RegisteredListener == a_otherRegisteredListener AND
	//Checks if this->Event == a_pRegisteredlistener->Event
	//Checks if this->Listener == a_pRegisteredlistener->Listener
	bool EventHandler::RegisteredListener:: operator==(const RegisteredListener& a_pRegisteredlistener)
	{
		return (this->m_pEventListener == a_pRegisteredlistener.m_pEventListener &&
			this->m_pIEvent == a_pRegisteredlistener.m_pIEvent);		
	}

	//Checks if this->Event == a_pRegisteredlistener->Event AND
	//Checks if this->Listener == a_pRegisteredlistener->Listener
	//Returns true if RegisteredListener != a_otherRegisteredListener
	bool EventHandler::RegisteredListener::operator!=(const RegisteredListener& a_pRegisteredlistener)
	{
		return !(*this == a_pRegisteredlistener);
	}

#pragma endregion Registered Listener
}
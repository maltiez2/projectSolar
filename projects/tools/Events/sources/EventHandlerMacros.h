#pragma once

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define EVENT_DECL(eventName, eventId, ...) enum : uint8_t {eventName = eventId}; struct PPCAT(eventName, _DATA) {__VA_ARGS__;}
#define EVENTS_DEF_UNKNOWN() case 0: LOG_ASSERT(false, "Unknown event type") {
#define EVENTS_DEF_DEFAULT() break; } default:
#define EVENT_DEF(eventName) break; } case eventName: { PPCAT(eventName, _DATA)& eventData = *(PPCAT(eventName, _DATA)*)data
#define SEND_EVENT(eventName, destClass, dest, ...) destClass::PPCAT(eventName, _DATA) PPCAT(data, __LINE__)({ __VA_ARGS__ }); (dest)->receiveEvent(destClass::eventName, PPCAT(data, __LINE__))
#define SUBSCRIPTION_DEF(eventName) case eventName: { PPCAT(eventName, _DATA)& eventData = *(PPCAT(eventName, _DATA)*)data; std::shared_lock readLock(m_subsritpionMutex); for (auto subscriber : m_subscriptions[eventName]) {subscriber->receiveEvent(eventName, eventData);}} break
#define SUBSCRIPTION_DEF_DEFAULT default: LOG_ERROR("[SubscriptionManager] Unknown subsricption event"); break
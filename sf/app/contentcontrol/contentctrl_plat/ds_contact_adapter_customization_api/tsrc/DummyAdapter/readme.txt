What is DummyAdapter?

Dummy adapter is a ECOM plugin, which acts as a stub implementation for Adapter Customization API. It is a SyncML adapter, which fully implements Adapter Customization API interface, but does not extend or restrict the default functionality provided by platform. Dummy adapter works identically compared to platform contact adapter. It enables to synchronize contacts with SyncML servers, such as Nokia OVI server, using vCards.

Dummy adapter can be also used as a skeleton for new operator adapters. This will reduce the work effort to implement new operator specific adapters compared to starting implementation from the scratch.


How to synchronize with OVI server using Dummy adapter?


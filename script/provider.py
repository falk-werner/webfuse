#!/usr/bin/env python3

import asyncio
import websockets

async def hello():
    async with websockets.connect('ws://localhost:8081') as websocket:
            await websocket.send('Hello')
            await websocket.recv()

asyncio.run(hello())

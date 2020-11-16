# RhinoDB
## Simple Key Value Store

KV store will support Get, Insert and Delete operations. It is using LevelDB as storage engine and GRPC as the network interface.

## Design

<p align="center">
  <img  src="doc/rhino.png">
</p>

## Features

* **High Performance**
* **High Scalability**
* **High Reliability**


## Checklist

| Crate     | Description |  Status |
|-----------|-------------|-------------|
| storage_engine | Implement RocksDB | TODO |
| transactions | Implement MultiVersion Concurrency Control | WIP |
| distributed | Add RaftGroups | TODO |
| client | Add a basic cli client | TODO |
| indexing | Support GSI | TODO |

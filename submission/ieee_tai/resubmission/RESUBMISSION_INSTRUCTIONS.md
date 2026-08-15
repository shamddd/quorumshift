# IEEE TAI Resubmission Step-by-Step Instructions (`RESUBMISSION_INSTRUCTIONS.md`)
**Manuscript ID**: `TAI-2026-Aug-A-01875`  
**Manuscript Title**: *AdaptiveReplica: Dynamic Quorum Adaptation and Failure-Aware Replica Selection in Distributed Consensus*

---

## 1. File Upload Mapping for IEEE ScholarOne

When accessing the submission portal at `https://mc.manuscriptcentral.com/tai-ieee` for **TAI-2026-Aug-A-01875**:

| ScholarOne File Designation | Local File Path to Upload | Purpose / Requirements Met |
| :--- | :--- | :--- |
| **Title Page** (`.docx`) | [`submission/ieee_tai/resubmission/AdaptiveReplica_TAI_Title_Page.docx`](file:///Users/shamthakare/.gemini/antigravity/scratch/quorumshift/submission/ieee_tai/resubmission/AdaptiveReplica_TAI_Title_Page.docx) | Contains author name (*Sham Satish Thakare*), email (*shamthakare3000@gmail.com*), affiliation, acknowledgments, and IEEE AI disclosure. Not sent to double-blind reviewers. |
| **Anonymized Main Document - PDF** (`.pdf`) | [`submission/ieee_tai/resubmission/AdaptiveReplica_TAI_Main_Manuscript.pdf`](file:///Users/shamthakare/.gemini/antigravity/scratch/quorumshift/submission/ieee_tai/resubmission/AdaptiveReplica_TAI_Main_Manuscript.pdf) | Double-column, single-spaced IEEE layout, 100% anonymized, 151-word abstract, 105-word impact statement, Table I, equations (1)-(2), and 9 verified references. |
| **Anonymized Main Document - MS Word** (`.docx`) *(Optional)* | [`submission/ieee_tai/resubmission/AdaptiveReplica_TAI_Main_Manuscript.docx`](file:///Users/shamthakare/.gemini/antigravity/scratch/quorumshift/submission/ieee_tai/resubmission/AdaptiveReplica_TAI_Main_Manuscript.docx) | Populated directly from the official `TAI_Word_Template.doc` using native IEEE paragraph styles (`Title`, `Abstract`, `IndexTerms`, `Heading 1`, `Text`). |

---

## 2. Portal Form Fields Copy-Paste Reference

### Title:
```text
AdaptiveReplica: Dynamic Quorum Adaptation and Failure-Aware Replica Selection in Distributed Consensus
```

### Abstract (151 Words):
```text
In fault-tolerant distributed storage systems, static majority quorums (R = 3, 5) suffer severe p99 tail-latency degradation under asymmetric network partitions and node slowdowns. While static configuration changes allow node additions or removals, they cannot dynamically adjust quorum voting weights in response to microsecond-scale network degradation without risking consistency violations or liveness starvation. In this paper, we present AdaptiveReplica, a dynamic quorum adaptation framework executing over Raft joint-consensus configuration transitions. AdaptiveReplica continuously monitors replica link latency, packet loss, and processing jitter, dynamically adjusting replica vote weights to bypass degraded nodes while maintaining strong consistency (C = 100%). Evaluated under 50ms asymmetric network fault injection across multi-seed benchmarks (N = 5), AdaptiveReplica achieves 99.97% system availability and reduces write p99 tail latency to 13.50ms (88.8% reduction compared to static R=5 majority consensus at 120.48ms), while guaranteeing zero stale reads (S_stale = 0). All code and experimental artifacts are open-source and fully reproducible.
```

### Impact Statement (105 Words):
```text
Distributed storage systems and cloud databases power essential modern computational infrastructure, from banking networks to autonomous AI decision systems. However, transient network latency spikes and node slowdowns frequently cause severe tail-latency amplification under traditional static quorum consensus protocols. The failure-aware dynamic quorum adaptation framework introduced in this paper overcomes these operational bottlenecks by automatically adjusting replica voting weights in real-time. By achieving an 88.8% reduction in write p99 tail latency while maintaining 100% strong consistency and 99.97% availability, this technology provides immediate practical benefits for latency-critical distributed key-value stores, cloud databases, and real-time AI orchestration engines without requiring expensive hardware upgrades or risking data corruption.
```

### Keywords / Index Terms:
* Artificial intelligence
* Autonomous agent infrastructure
* Distributed consensus
* Fault tolerance
* Quorum adaptation
* Raft protocol
* Reliability
* Tail latency

# Machine Learning Paper Trader

This repository implements a real-time trading simulator that uses a logistic regression binary classifier on market microstructure data to predict short-term price movements. The system spans offline model training in Python and real-time order book inference and execution in C++. This simulator can be used to test the performance of any trained logistic regression binary classifier. Each new model requires a change in `feature_engineering()` to create the appropriate inference vector.

---

## 📊 Research & Model Development

Model development is conducted in [`research.ipynb`](./research.ipynb). The goal is to classify whether the mid-price will move **up** or **down** over the next 20 order book updates using features derived from Level-2 order book snapshots. I trained a logistic regression because the equation underlying a logistic regression model can easily be written into a C++ program.

### Workflow:

1. **Load Historical Data**  
   Raw L2 order book snapshots are loaded and preprocessed into labeled data.

2. **Feature Engineering**  
   Features include:
   - Bid-ask spread
   - Price and volume at top N levels
   - Log-transformed volumes
   - Order book imbalance
   - VWAP and log VWAP

3. **Model Training**  
   A logistic regression classifier is trained to predict the direction of the mid-price movement (up/down) using the above features.

4. **Export Coefficients**  
   The trained model’s coefficients and intercept are exported to a text file: 20_update_log_classifier.txt


## ⚙️ Real-Time Inference & Trading

The live simulation is performed in C++ by combining:
- A `LogisticRegressionModel` (from exported coefficients)
- WebSocket-based real-time data stream (via `websocket.py`)
- A trading simulation engine (`simulator.cpp`)

### Real-Time Workflow:

1. **Launch WebSocket**
simulator.cpp spawns websocket.py in the background. This Python script:

Connects to the Coinbase WebSocket API

Streams real-time L2 updates for a product like BTC-USD

Writes updates to a named pipe at /tmp/datapipeline

2. **Ingest & Update Order Book**
The C++ simulator reads messages from the pipe, parses the JSON, and updates the order book with orderbook.cpp.

3. **Feature Generation**
Every message triggers the feature generation function in simulator.cpp, which extracts market microstructure features. Done in `feature_engineering()`, which returns the inference vector based on the updated orderbook microstructure.

4. **Inference**
These features are passed into the LogisticRegressionModel (from model.cpp) to compute the probability of an upward mid-price movement.

5. **Position Management**
If the probability exceeds a confidence threshold (e.g., 0.6), the simulator simulates a market buy.

It then holds the position for a fixed number of updates (update_lag), then simulates a sell.

Trade details (buy/sell amount, volume, PnL) are stored in position.cpp and logged to a timestamped .csv file in /simulations.

6. **Analysis**
Model performance can be analyzed in simulation_analysis.ipynb












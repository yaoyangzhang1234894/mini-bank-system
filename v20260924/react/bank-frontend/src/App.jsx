import React, { useState, useEffect } from 'react';
import './App.css';
import * as api from './api';

function App() {
  let accountsState = useState([]);
  let accounts = accountsState[0];
  let setAccounts = accountsState[1];

  let loadingState = useState(false);
  let loading = loadingState[0];
  let setLoading = loadingState[1];

  let activeTabState = useState('dashboard');
  let activeTab = activeTabState[0];
  let setActiveTab = activeTabState[1];

  let notifState = useState(null);
  let notif = notifState[0];
  let setNotif = notifState[1];

  let createState = useState({ holderName: '', initialBalance: 0, currency: 'CAD', userName: '', passWord: '' });
  let createF = createState[0];
  let setCreateF = createState[1];

  let depositState = useState({ userName: '', passWord: '', amount: 100 });
  let depositF = depositState[0];
  let setDepositF = depositState[1];

  let withdrawState = useState({ userName: '', passWord: '', amount: 100 });
  let withdrawF = withdrawState[0];
  let setWithdrawF = withdrawState[1];

  let transferState = useState({ fromUserName: '', fromPassword: '', toUserName: '', toPassword: '', amount: 100 });
  let transferF = transferState[0];
  let setTransferF = transferState[1];

  let deleteState = useState({ userName: '', passWord: '' });
  let deleteF = deleteState[0];
  let setDeleteF = deleteState[1];

 
  function showMsg(msg, type) {
    if (type === undefined) {
      type = 'info';
    }
    setNotif({ msg: msg, type: type });
    setTimeout(function() {
      setNotif(null);
    }, 3000);
  }


  function loadData() {
    setLoading(true);
    api.getAccounts()
      .then(function(res) {
        var data = res.data.data || [];
        setAccounts(data);
        setLoading(false);
      })
      .catch(function(error) {
        showMsg('/Connection to the backend failed. Please ensure the backend is running. (./bank_server)', 'error');
        setLoading(false);
      });
  }


  useEffect(function() {
    loadData();
  }, []);

  // ---- 创建账户 ----
  function handleCreate(e) {
    e.preventDefault();
    api.createAccount(createF)
      .then(function() {
        showMsg('账户创建成功/Account created successfully', 'success');
        loadData();
        setCreateF({ holderName: '', initialBalance: 0, currency: 'CAD', userName: '', passWord: '' });
      })
      .catch(function() {
        showMsg('创建失败/Creation failed', 'error');
      });
  }

  // ---- 存款 ----
  function handleDeposit(e) {
    e.preventDefault();
    api.deposit(depositF)
      .then(function(res) {
        if (res.data.success) {
          var newBal = res.data.newBalance;
          showMsg('Deposit successful! New balance(存款成功！新余额): $' + newBal, 'success');
          loadData();
          setDepositF({ userName: depositF.userName, passWord: depositF.passWord, amount: 100 });
        } else {
          showMsg('存款失败/Deposit failure', 'error');
        }
      })
      .catch(function() {
        showMsg('操作失败/failed', 'error');
      });
  }

  // ---- 取款 ----
  function handleWithdraw(e) {
    e.preventDefault();
    api.withdraw(withdrawF)
      .then(function(res) {
        if (res.data.success) {
          var newBal = res.data.newBalance;
          showMsg('Withdrawal successful! New balance(取款成功！新余额): $' + newBal, 'success');
          loadData();
          setWithdrawF({ userName: withdrawF.userName, passWord: withdrawF.passWord, amount: 100 });
        } else {
          showMsg('余额不足或操作失败/Insufficient balance or failed', 'error');
        }
      })
      .catch(function() {
        showMsg('操作失败/failed', 'error');
      });
  }

  // ---- 转账 ----
  function handleTransfer(e) {
    e.preventDefault();
    api.transfer(transferF)
      .then(function() {
        showMsg('转账成功/Transfer successful', 'success');
        loadData();
        setTransferF({ fromUserName: '', fromPassword: '', toUserName: '', toPassword: '', amount: 100 });
      })
      .catch(function() {
        showMsg('转账失败/Transfer failed', 'error');
      });
  }

  // ---- 删除 ----
  function handleDelete(e) {
    e.preventDefault();
    var confirmResult = window.confirm('确定永久删除吗？/Are you sure you want to permanently delete this?');
    if (!confirmResult) {
      return;
    }
    api.deleteAccount(deleteF)
      .then(function() {
        showMsg('已删除/Deleted', 'success');
        loadData();
        setDeleteF({ userName: '', passWord: '' });
      })
      .catch(function() {
        showMsg('删除失败/Deletion failed', 'error');
      });
  }


  var totalBalance = 0;
  for (var i = 0; i < accounts.length; i++) {
    totalBalance = totalBalance + accounts[i].balance;
  }
  var totalAccounts = accounts.length;


  var accountItems = [];
  if (accounts.length === 0) {
    accountItems.push(
      <p key="empty" style={{ textAlign: 'center', color: '#999', padding: '20px' }}>
        暂无数据，请创建账户/No data available. Please create an account.
      </p>
    );
  } else {
    for (var j = 0; j < accounts.length; j++) {
      var acc = accounts[j];
      var statusClass = acc.isActive ? 'active' : 'inactive';
      var statusText = acc.isActive ? '活跃/active' : '已关闭/inactive';
      accountItems.push(
        <div className="account-item" key={acc.accountId}>
          <div>
            <span className="name">{acc.accountHolderName}</span>
            <span className={'status-badge ' + statusClass}>{statusText}</span>
            <div className="detail">@{acc.userName}</div>
          </div>
          <div className="balance">${acc.balance.toFixed(2)}</div>
        </div>
      );
    }
  }


  var dashboardContent;
  if (activeTab === 'dashboard') {
    dashboardContent = (
      <>
        <div className="stats">
          <div className="stat-card">
            <div className="label">总账户/total Accounts</div>
            <div className="value">{totalAccounts}</div>
          </div>
          <div className="stat-card">
            <div className="label">总资产/tota lBalance</div>
            <div className="value">${totalBalance.toFixed(2)}</div>
          </div>
        </div>
        <div className="panel">
          <h2>账户列表/Account List</h2>
          <div className="account-list">
            {accountItems}
          </div>
        </div>
      </>
    );
  }

  // 账户管理内容
  var accountsContent;
  if (activeTab === 'accounts') {
    accountsContent = (
      <div className="grid-2">
        <div className="panel">
          <h2>创建账户/create Account</h2>
          <form onSubmit={handleCreate}>
            <input
              placeholder="持有人/holder Name"
              value={createF.holderName}
              onChange={function(e) { setCreateF({ holderName: e.target.value, initialBalance: createF.initialBalance, currency: createF.currency, userName: createF.userName, passWord: createF.passWord }); }}
              required
            />
            <input
              type="number"
              placeholder="初始余额/initial Balance"
              value={createF.initialBalance}
              onChange={function(e) { setCreateF({ holderName: createF.holderName, initialBalance: parseFloat(e.target.value), currency: createF.currency, userName: createF.userName, passWord: createF.passWord }); }}
              required
            />
            <input
              placeholder="币种/currency"
              value={createF.currency}
              onChange={function(e) { setCreateF({ holderName: createF.holderName, initialBalance: createF.initialBalance, currency: e.target.value, userName: createF.userName, passWord: createF.passWord }); }}
            />
            <input
              placeholder="用户名/userName"
              value={createF.userName}
              onChange={function(e) { setCreateF({ holderName: createF.holderName, initialBalance: createF.initialBalance, currency: createF.currency, userName: e.target.value, passWord: createF.passWord }); }}
              required
            />
            <input
              type="password"
              placeholder="密码/passWord"
              value={createF.passWord}
              onChange={function(e) { setCreateF({ holderName: createF.holderName, initialBalance: createF.initialBalance, currency: createF.currency, userName: createF.userName, passWord: e.target.value }); }}
              required
            />
            <button type="submit" className="btn-primary">创建/create</button>
          </form>
        </div>
        <div className="panel">
          <h2>关闭账户/Close account</h2>
          <form onSubmit={handleDelete}>
            <input
              placeholder="用户名/userName"
              value={deleteF.userName}
              onChange={function(e) { setDeleteF({ userName: e.target.value, passWord: deleteF.passWord }); }}
              required
            />
            <input
              type="password"
              placeholder="密码/passWord"
              value={deleteF.passWord}
              onChange={function(e) { setDeleteF({ userName: deleteF.userName, passWord: e.target.value }); }}
              required
            />
            <button type="submit" className="btn-danger">永久删除/Permanent deletion</button>
          </form>
        </div>
      </div>
    );
  }

  // 交易操作内容
  var transactionsContent;
  if (activeTab === 'transactions') {
    transactionsContent = (
      <div className="grid-2">
        <div className="panel">
          <h2>存款/Deposit</h2>
          <form onSubmit={handleDeposit}>
            <input
              placeholder="用户名/userName"
              value={depositF.userName}
              onChange={function(e) { setDepositF({ userName: e.target.value, passWord: depositF.passWord, amount: depositF.amount }); }}
              required
            />
            <input
              type="password"
              placeholder="密码/passWord"
              value={depositF.passWord}
              onChange={function(e) { setDepositF({ userName: depositF.userName, passWord: e.target.value, amount: depositF.amount }); }}
              required
            />
            <input
              type="number"
              placeholder="金额/amount"
              value={depositF.amount}
              onChange={function(e) { setDepositF({ userName: depositF.userName, passWord: depositF.passWord, amount: parseFloat(e.target.value) }); }}
              required
            />
            <button type="submit" className="btn-success">存款/deposit</button>
          </form>
        </div>
        <div className="panel">
          <h2>取款/Withdraw</h2>
          <form onSubmit={handleWithdraw}>
            <input
              placeholder="用户名/userName"
              value={withdrawF.userName}
              onChange={function(e) { setWithdrawF({ userName: e.target.value, passWord: withdrawF.passWord, amount: withdrawF.amount }); }}
              required
            />
            <input
              type="password"
              placeholder="密码/passWord"
              value={withdrawF.passWord}
              onChange={function(e) { setWithdrawF({ userName: withdrawF.userName, passWord: e.target.value, amount: withdrawF.amount }); }}
              required
            />
            <input
              type="number"
              placeholder="金额/amount"
              value={withdrawF.amount}
              onChange={function(e) { setWithdrawF({ userName: withdrawF.userName, passWord: withdrawF.passWord, amount: parseFloat(e.target.value) }); }}
              required
            />
            <button type="submit" className="btn-danger">取款/withdrawal</button>
          </form>
        </div>
      </div>
    );
  }

  // 转账内容
  var transferContent;
  if (activeTab === 'transfer') {
    transferContent = (
      <div className="grid-2">
        <div className="panel">
          <h2>转账/Transfer</h2>
          <form onSubmit={handleTransfer}>
            <input
              placeholder="转出用户名/from UserName"
              value={transferF.fromUserName}
              onChange={function(e) { setTransferF({ fromUserName: e.target.value, fromPassword: transferF.fromPassword, toUserName: transferF.toUserName, toPassword: transferF.toPassword, amount: transferF.amount }); }}
              required
            />
            <input
              type="password"
              placeholder="转出密码/from Passworld"
              value={transferF.fromPassword}
              onChange={function(e) { setTransferF({ fromUserName: transferF.fromUserName, fromPassword: e.target.value, toUserName: transferF.toUserName, toPassword: transferF.toPassword, amount: transferF.amount }); }}
              required
            />
            <hr className="divider" />
            <input
              placeholder="转入用户名/to UserName"
              value={transferF.toUserName}
              onChange={function(e) { setTransferF({ fromUserName: transferF.fromUserName, fromPassword: transferF.fromPassword, toUserName: e.target.value, toPassword: transferF.toPassword, amount: transferF.amount }); }}
              required
            />
            <input
              type="password"
              placeholder="转入密码/to Passworld"
              value={transferF.toPassword}
              onChange={function(e) { setTransferF({ fromUserName: transferF.fromUserName, fromPassword: transferF.fromPassword, toUserName: transferF.toUserName, toPassword: e.target.value, amount: transferF.amount }); }}
              required
            />
            <hr className="divider" />
            <input
              type="number"
              placeholder="金额/amount"
              value={transferF.amount}
              onChange={function(e) { setTransferF({ fromUserName: transferF.fromUserName, fromPassword: transferF.fromPassword, toUserName: transferF.toUserName, toPassword: transferF.toPassword, amount: parseFloat(e.target.value) }); }}
              required
            />
            <button type="submit" className="btn-purple">执行转账/Executing transfer</button>
          </form>
        </div>
        <div className="panel">
          <h2>提示/note</h2>
          <p style={{ color: '#666' }}>temp reserve place</p>
        </div>
      </div>
    );
  }

  // 主界面返回
  return (
    <div className="app">
      {notif !== null && <div className={'notification ' + notif.type}>{notif.msg}</div>}

      <div className="navbar">
        <h1>智能银行系统/Smart Banking System</h1>
        <button onClick={loadData} disabled={loading}>
          {loading ? 'loading...' : '刷新/refresh'}
        </button>
      </div>

      <div className="tabs">
        <button
          className={activeTab === 'dashboard' ? 'active' : ''}
          onClick={function() { setActiveTab('dashboard'); }}
        >
          仪表盘
        </button>
        <button
          className={activeTab === 'accounts' ? 'active' : ''}
          onClick={function() { setActiveTab('accounts'); }}
        >
          账户管理
        </button>
        <button
          className={activeTab === 'transactions' ? 'active' : ''}
          onClick={function() { setActiveTab('transactions'); }}
        >
          交易
        </button>
        <button
          className={activeTab === 'transfer' ? 'active' : ''}
          onClick={function() { setActiveTab('transfer'); }}
        >
          转账
        </button>
      </div>

      {dashboardContent}
      {accountsContent}
      {transactionsContent}
      {transferContent}
    </div>
  );
}

export default App;